/*
 * s3.cpp
 *
 *  Created on: 2018年2月26日
 *      Author: liaoyp
 */
#include "s3.h"
#include "config.h"
#include "logging4.h"

INIT_LOGGER("S3");

ceph::S3 * ceph::S3::instance = NULL;

ceph::S3::S3():user(NULL),host(NULL),accessKey(NULL),secretKey(NULL){};

ceph::S3::~S3(){}

ceph::S3 * ceph::S3::getInstance()
{
	instance = new ceph::S3();
	return instance;
}

int ceph::S3::S3_init(int type)
{
	if(!parseData(type))
	{
		_LOG_ERROR("\nERROR: parse data error.\n");
		return StatusXmlParseFailure;
	}
	accessKeyIdG = accessKey;
	if (!accessKeyIdG) {
		_LOG_ERROR("Missing variable: S3_ACCESS_KEY_ID\n");
		return StatusInvalidArgument;
	}
	secretAccessKeyG = secretKey;
	if (!secretAccessKeyG) {
		_LOG_ERROR("Missing variable: S3_SECRET_ACCESS_KEY\n");
		return StatusInvalidArgument;
	}
	S3Status status;
	if ((status = S3_initialize("s3", S3_INIT_ALL, host))
		!= S3StatusOK) {
		_LOG_ERROR("Failed to initialize libs3: %s\n" <<
				S3_get_status_name(status));
		return StatusRequestFailed;
	}
	return StatusOK;
}

void ceph::S3::printError(void)
{
    if (statusG < S3StatusErrorAccessDenied) {
    	_LOG_ERROR("\nERROR: %s\n" << S3_get_status_name((S3Status)statusG));
    }
    else {
    	_LOG_ERROR("\nERROR: %s\n" << S3_get_status_name((S3Status)statusG));
    	_LOG_ERROR("%s\n" << errorDetailsG);
    }
}

uint64_t ceph::S3::convertInt(const char *str, const char *paramName)
{
	uint64_t ret = 0;

	while (*str) {
		if (!isdigit(*str)) {
			_LOG_ERROR("\nERROR: Nondigit in %s parameter: %c\n" <<	paramName << *str);
		}
		ret *= 10;
		ret += (*str++ - '0');
	}

	return ret;
}

int ceph::S3::growbuffer_append(growbuffer **gb, const char *data, int dataLen)
{
	while (dataLen) {
	growbuffer *buf = *gb ? (*gb)->prev : 0;
	if (!buf || (buf->size == sizeof(buf->data))) {
		buf = (growbuffer *) malloc(sizeof(growbuffer));
		if (!buf) {
			return 0;
		}
		buf->size = 0;
		buf->start = 0;
		if (*gb && (*gb)->prev) {
			buf->prev = (*gb)->prev;
			buf->next = *gb;
			(*gb)->prev->next = buf;
			(*gb)->prev = buf;
		}
		else {
			buf->prev = buf->next = buf;
			*gb = buf;
		}
	}

	int toCopy = (sizeof(buf->data) - buf->size);
	if (toCopy > dataLen) {
		toCopy = dataLen;
	}

	memcpy(&(buf->data[buf->size]), data, toCopy);

	buf->size += toCopy, data += toCopy, dataLen -= toCopy;
}

return 1;
}

void ceph::S3::growbuffer_read(growbuffer **gb, int amt, int *amtReturn, char *buffer)
{
	*amtReturn = 0;

	growbuffer *buf = *gb;

	if (!buf) {
		return;
	}

	*amtReturn = (buf->size > amt) ? amt : buf->size;

	memcpy(buffer, &(buf->data[buf->start]), *amtReturn);

	buf->start += *amtReturn, buf->size -= *amtReturn;

	if (buf->size == 0) {
		if (buf->next == buf) {
			*gb = 0;
		}
		else {
			*gb = buf->next;
			buf->prev->next = buf->next;
			buf->next->prev = buf->prev;
		}
		free(buf);
	}
}

void ceph::S3::growbuffer_destroy(growbuffer *gb)
{
	growbuffer *start = gb;

	while (gb) {
		growbuffer *next = gb->next;
		free(gb);
		gb = (next == start) ? 0 : next;
	}
}

int ceph::S3::checkString(const char *str, const char *format)
{
	while (*format) {
		if (*format == 'd') {
			if (!isdigit(*str)) {
				return 0;
			}
		}
		else if (*str != *format) {
			return 0;
		}
		str++, format++;
	}

	return 1;
}

int64_t ceph::S3::parseIso8601Time(const char *str)
{
	// Check to make sure that it has a valid format
	if (!checkString(str, "dddd-dd-ddTdd:dd:dd")) {
		return -1;
	}

#define nextnum() (((*str - '0') * 10) + (*(str + 1) - '0'))

	// Convert it
	struct tm stm;
	memset(&stm, 0, sizeof(stm));

	stm.tm_year = (nextnum() - 19) * 100;
	str += 2;
	stm.tm_year += nextnum();
	str += 3;

	stm.tm_mon = nextnum() - 1;
	str += 3;

	stm.tm_mday = nextnum();
	str += 3;

	stm.tm_hour = nextnum();
	str += 3;

	stm.tm_min = nextnum();
	str += 3;

	stm.tm_sec = nextnum();
	str += 2;

	stm.tm_isdst = -1;

	// This is hokey but it's the recommended way ...
	char *tz = getenv("TZ");
	snprintf(putenvBufG, sizeof(putenvBufG), "TZ=UTC");
	putenv(putenvBufG);

	int64_t ret = mktime(&stm);

	snprintf(putenvBufG, sizeof(putenvBufG), "TZ=%s", tz ? tz : "");
	putenv(putenvBufG);

	// Skip the millis

	if (*str == '.') {
		str++;
		while (isdigit(*str)) {
			str++;
		}
	}

	if (checkString(str, "-dd:dd") || checkString(str, "+dd:dd")) {
		int sign = (*str++ == '-') ? -1 : 1;
		int hours = nextnum();
		str += 3;
		int minutes = nextnum();
		ret += (-sign * (((hours * 60) + minutes) * 60));
	}
	// Else it should be Z to be a conformant time string, but we just assume
	// that it is rather than enforcing that

	return ret;
}

int ceph::S3::convert_simple_acl(char *aclXml, char *ownerId,
	                              char *ownerDisplayName,
	                              int *aclGrantCountReturn,
	                              S3AclGrant *aclGrants)
{
	*aclGrantCountReturn = 0;
	*ownerId = 0;
	*ownerDisplayName = 0;

#define SKIP_SPACE(require_more)                \
	do {                                        \
		while (isspace(*aclXml)) {              \
			aclXml++;                           \
		}                                       \
		if (require_more && !*aclXml) {         \
			return 0;                           \
		}                                       \
	} while (0)

#define COPY_STRING_MAXLEN(field, maxlen)               \
	do {                                                \
		SKIP_SPACE(1);                                  \
		int len = 0;                                    \
		while ((len < maxlen) && !isspace(*aclXml)) {   \
			field[len++] = *aclXml++;                   \
		}                                               \
		field[len] = 0;                                 \
	} while (0)

#define COPY_STRING(field)                              \
	COPY_STRING_MAXLEN(field, (int) (sizeof(field) - 1))

	while (1) {
		SKIP_SPACE(0);

		if (!*aclXml) {
			break;
		}

		// Skip Type lines and dash lines
		if (!strncmp(aclXml, "Type", sizeof("Type") - 1) ||
			(*aclXml == '-')) {
			while (*aclXml && ((*aclXml != '\n') && (*aclXml != '\r'))) {
				aclXml++;
			}
			continue;
		}

		if (!strncmp(aclXml, "OwnerID", sizeof("OwnerID") - 1)) {
			aclXml += sizeof("OwnerID") - 1;
			COPY_STRING_MAXLEN(ownerId, S3_MAX_GRANTEE_USER_ID_SIZE);
			SKIP_SPACE(1);
			COPY_STRING_MAXLEN(ownerDisplayName,
							   S3_MAX_GRANTEE_DISPLAY_NAME_SIZE);
			continue;
		}

		if (*aclGrantCountReturn == S3_MAX_ACL_GRANT_COUNT) {
			return 0;
		}

		S3AclGrant *grant = &(aclGrants[(*aclGrantCountReturn)++]);

		if (!strncmp(aclXml, "Email", sizeof("Email") - 1)) {
			grant->granteeType = S3GranteeTypeAmazonCustomerByEmail;
			aclXml += sizeof("Email") - 1;
			COPY_STRING(grant->grantee.amazonCustomerByEmail.emailAddress);
		}
		else if (!strncmp(aclXml, "UserID", sizeof("UserID") - 1)) {
			grant->granteeType = S3GranteeTypeCanonicalUser;
			aclXml += sizeof("UserID") - 1;
			COPY_STRING(grant->grantee.canonicalUser.id);
			SKIP_SPACE(1);
			// Now do display name
			COPY_STRING(grant->grantee.canonicalUser.displayName);
		}
		else if (!strncmp(aclXml, "Group", sizeof("Group") - 1)) {
			aclXml += sizeof("Group") - 1;
			SKIP_SPACE(1);
			if (!strncmp(aclXml, "Authenticated AWS Users",
						 sizeof("Authenticated AWS Users") - 1)) {
				grant->granteeType = S3GranteeTypeAllAwsUsers;
				aclXml += (sizeof("Authenticated AWS Users") - 1);
			}
			else if (!strncmp(aclXml, "All Users", sizeof("All Users") - 1)) {
				grant->granteeType = S3GranteeTypeAllUsers;
				aclXml += (sizeof("All Users") - 1);
			}
			else if (!strncmp(aclXml, "Log Delivery",
							  sizeof("Log Delivery") - 1)) {
				grant->granteeType = S3GranteeTypeLogDelivery;
				aclXml += (sizeof("Log Delivery") - 1);
			}
			else {
				return 0;
			}
		}
		else {
			return 0;
		}

		SKIP_SPACE(1);

		if (!strncmp(aclXml, "READ_ACP", sizeof("READ_ACP") - 1)) {
			grant->permission = S3PermissionReadACP;
			aclXml += (sizeof("READ_ACP") - 1);
		}
		else if (!strncmp(aclXml, "READ", sizeof("READ") - 1)) {
			grant->permission = S3PermissionRead;
			aclXml += (sizeof("READ") - 1);
		}
		else if (!strncmp(aclXml, "WRITE_ACP", sizeof("WRITE_ACP") - 1)) {
			grant->permission = S3PermissionWriteACP;
			aclXml += (sizeof("WRITE_ACP") - 1);
		}
		else if (!strncmp(aclXml, "WRITE", sizeof("WRITE") - 1)) {
			grant->permission = S3PermissionWrite;
			aclXml += (sizeof("WRITE") - 1);
		}
		else if (!strncmp(aclXml, "FULL_CONTROL",
						  sizeof("FULL_CONTROL") - 1)) {
			grant->permission = S3PermissionFullControl;
			aclXml += (sizeof("FULL_CONTROL") - 1);
		}
	}

	return 1;
}

int ceph::S3::should_retry()
{
	if (retriesG--) {
		// Sleep before next retry; start out with a 1 second sleep
		static int retrySleepInterval = 1 * SLEEP_UNITS_PER_SECOND;
		sleep(retrySleepInterval);
		// Next sleep 1 second longer
		retrySleepInterval++;
		return 1;
	}

	return 0;
}

S3Status ceph::S3::responsePropertiesCallback
	    (const S3ResponseProperties *properties, void *callbackData)
{
	(void) callbackData;

	if (!showResponsePropertiesG) {
		return S3StatusOK;
	}

#define print_nonnull(name, field)                                 \
	do {                                                           \
		if (properties-> field) {                                  \
			_LOG_DEBUG("%s: %s\n" << name << properties-> field);      \
		}                                                          \
	} while (0)

	print_nonnull("Content-Type", contentType);
	print_nonnull("Request-Id", requestId);
	print_nonnull("Request-Id-2", requestId2);
	if (properties->contentLength > 0) {
		_LOG_DEBUG("Content-Length: %lld\n" <<
			   (unsigned long long) properties->contentLength);
	}
	print_nonnull("Server", server);
	print_nonnull("ETag", eTag);
	if (properties->lastModified > 0) {
		char timebuf[256];
		time_t t = (time_t) properties->lastModified;
		// gmtime is not thread-safe but we don't care here.
		strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));
		_LOG_DEBUG("Last-Modified: %s\n" << timebuf);
	}
	int i;
	for (i = 0; i < properties->metaDataCount; i++) {
		_LOG_DEBUG("x-amz-meta-%s: %s\n" << properties->metaData[i].name <<
			   properties->metaData[i].value);
	}

	return S3StatusOK;
}

void ceph::S3::responseCompleteCallback(S3Status status,
	                                     const S3ErrorDetails *error,
	                                     void *callbackData)
{
	(void) callbackData;

	statusG = status;
	// Compose the error details message now, although we might not use it.
	// Can't just save a pointer to [error] since it's not guaranteed to last
	// beyond this callback
	int len = 0;
	if (error && error->message) {
		len += snprintf(&(errorDetailsG[len]), sizeof(errorDetailsG) - len,
						"  Message: %s\n", error->message);
	}
	if (error && error->resource) {
		len += snprintf(&(errorDetailsG[len]), sizeof(errorDetailsG) - len,
						"  Resource: %s\n", error->resource);
	}
	if (error && error->furtherDetails) {
		len += snprintf(&(errorDetailsG[len]), sizeof(errorDetailsG) - len,
						"  Further Details: %s\n", error->furtherDetails);
	}
	if (error && error->extraDetailsCount) {
		len += snprintf(&(errorDetailsG[len]), sizeof(errorDetailsG) - len,
						"%s", "  Extra Details:\n");
		int i;
		for (i = 0; i < error->extraDetailsCount; i++) {
			len += snprintf(&(errorDetailsG[len]),
							sizeof(errorDetailsG) - len, "    %s: %s\n",
							error->extraDetails[i].name,
							error->extraDetails[i].value);
		}
	}
}

S3Status ceph::S3::listServiceCallback(const char *ownerId,
	                                    const char *ownerDisplayName,
	                                    const char *bucketName,
	                                    int64_t creationDate, void *callbackData)
{
	list_service_data *data = (list_service_data *) callbackData;

	if (!data->headerPrinted) {
		data->headerPrinted = 1;
		printListServiceHeader(data->allDetails);
	}

	char timebuf[256];
	if (creationDate >= 0) {
		time_t t = (time_t) creationDate;
		strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%SZ", gmtime(&t));
	}
	else {
		timebuf[0] = 0;
	}

	_LOG_DEBUG("%-56s  %-20s" << bucketName << timebuf);
	if (data->allDetails) {
		ownerId = ownerId ? ownerId : "";
		ownerDisplayName = ownerDisplayName ? ownerDisplayName : "";
		_LOG_DEBUG("  %-64s  %-12s" << ownerId << ownerDisplayName);
	}
	_LOG_DEBUG("\n");

	return S3StatusOK;
}

S3Status ceph::S3::listBucketCallback(int isTruncated, const char *nextMarker,
	                                   int contentsCount,
	                                   const S3ListBucketContent *contents,
	                                   int commonPrefixesCount,
	                                   const char **commonPrefixes,
	                                   void *callbackData)
{
	list_bucket_callback_data *data =
	        (list_bucket_callback_data *) callbackData;

	data->isTruncated = isTruncated;
	// This is tricky.  S3 doesn't return the NextMarker if there is no
	// delimiter.  Why, I don't know, since it's still useful for paging
	// through results.  We want NextMarker to be the last content in the
	// list, so set it to that if necessary.
	if ((!nextMarker || !nextMarker[0]) && contentsCount) {
		nextMarker = contents[contentsCount - 1].key;
	}
	if (nextMarker) {
		snprintf(data->nextMarker, sizeof(data->nextMarker), "%s",
				 nextMarker);
	}
	else {
		data->nextMarker[0] = 0;
	}

	if (contentsCount && !data->keyCount) {
		printListBucketHeader(data->allDetails);
	}

	int i;
	for (i = 0; i < contentsCount; i++) {
		const S3ListBucketContent *content = &(contents[i]);
		char timebuf[256];
		if (0) {
			time_t t = (time_t) content->lastModified;
			strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%SZ",
					 gmtime(&t));
			_LOG_DEBUG("\nKey: %s\n" << content->key);
			_LOG_DEBUG("Last Modified: %s\n" << timebuf);
			_LOG_DEBUG("ETag: %s\n" << content->eTag);
			_LOG_DEBUG("Size: %llu\n" << (unsigned long long) content->size);
			if (content->ownerId) {
				_LOG_DEBUG("Owner ID: %s\n" << content->ownerId);
			}
			if (content->ownerDisplayName) {
				_LOG_DEBUG("Owner Display Name: %s\n" << content->ownerDisplayName);
			}
		}
		else {
			time_t t = (time_t) content->lastModified;
			strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%SZ",
					 gmtime(&t));
			char sizebuf[16];
			if (content->size < 100000) {
				sprintf(sizebuf, "%5llu", (unsigned long long) content->size);
			}
			else if (content->size < (1024 * 1024)) {
				sprintf(sizebuf, "%4lluK",
						((unsigned long long) content->size) / 1024ULL);
			}
			else if (content->size < (10 * 1024 * 1024)) {
				float f = content->size;
				f /= (1024 * 1024);
				sprintf(sizebuf, "%1.2fM", f);
			}
			else if (content->size < (1024 * 1024 * 1024)) {
				sprintf(sizebuf, "%4lluM",
						((unsigned long long) content->size) /
						(1024ULL * 1024ULL));
			}
			else {
				float f = (content->size / 1024);
				f /= (1024 * 1024);
				sprintf(sizebuf, "%1.2fG", f);
			}
			_LOG_DEBUG("%-50s  %s  %s" << content->key << timebuf << sizebuf);
			if (data->allDetails) {
				const char * oId = content->ownerId ? content->ownerId : "";
				const char * oName = content->ownerDisplayName ? content->ownerDisplayName : "";
				_LOG_DEBUG("  %-34s  %-64s  %-12s" <<
					   content->eTag << oId << oName);
			}
			_LOG_DEBUG("\n");
		}
	}

	data->keyCount += contentsCount;

	for (i = 0; i < commonPrefixesCount; i++) {
		_LOG_DEBUG("\nCommon Prefix: %s\n" << commonPrefixes[i]);
	}

	return S3StatusOK;
}

int ceph::S3::putObjectDataCallback(int bufferSize, char *buffer,
	                                 void *callbackData)
{
	put_object_callback_data *data =
	        (put_object_callback_data *) callbackData;

	int ret = 0;

	if (data->contentLength) {
		int toRead = ((data->contentLength > (unsigned) bufferSize) ?
					  (unsigned) bufferSize : data->contentLength);
		if (data->gb) {
			growbuffer_read(&(data->gb), toRead, &ret, buffer);
		}
		else if (data->infile) {
			ret = fread(buffer, 1, toRead, data->infile);
		}
	}

	data->contentLength -= ret;

//	if (data->contentLength && !data->noStatus) {
//		// Avoid a weird bug in MingW, which won't print the second integer
//		// value properly when it's in the same call, so print separately
//		printf("%llu bytes remaining ",
//			   (unsigned long long) data->contentLength);
//		printf("(%d%% complete) ...\n",
//			   (int) (((data->originalContentLength -
//						data->contentLength) * 100) /
//					  data->originalContentLength));
//	}

	return ret;
}

S3Status ceph::S3::getObjectDataCallback(int bufferSize, const char *buffer,
	                                      void *callbackData)
{
	FILE *outfile = (FILE *) callbackData;

	size_t wrote = fwrite(buffer, 1, bufferSize, outfile);

	return ((wrote < (size_t) bufferSize) ?
			S3StatusAbortedByCallback : S3StatusOK);
}

void ceph::S3::printListServiceHeader(int allDetails)
{
	_LOG_DEBUG("%-56s  %-20s" << "                         Bucket" <<
	           "      Created");
	if (allDetails) {
		_LOG_DEBUG("  %-64s  %-12s" <<
			   "                            Owner ID" <<
			   "Display Name");
	}
	_LOG_DEBUG("\n");
	_LOG_DEBUG("--------------------------------------------------------  "
		   "--------------------");
	if (allDetails) {
		_LOG_DEBUG("  -------------------------------------------------"
			   "---------------  ------------");
	}
	_LOG_DEBUG("\n");
}

void ceph::S3::printListBucketHeader(int allDetails)
{
	_LOG_DEBUG("%-50s  %-20s  %-5s" <<
	           "                       Key" <<
	           "   Last Modified" << "Size");
	if (allDetails) {
		_LOG_DEBUG("  %-34s  %-64s  %-12s" <<
			   "               ETag" <<
			   "                            Owner ID" <<
			   "Display Name");
	}
	_LOG_DEBUG("\n");
	_LOG_DEBUG("--------------------------------------------------  "
		   "--------------------  -----");
	if (allDetails) {
		_LOG_DEBUG("  ----------------------------------  "
			   "-------------------------------------------------"
			   "---------------  ------------");
	}
	_LOG_DEBUG("\n");
}

int ceph::S3::list_service(int allDetails, const int type)
{
	list_service_data data;

	data.headerPrinted = 0;
	data.allDetails = allDetails;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3ListServiceHandler listServiceHandler =
	{
		{ &responsePropertiesCallback, &responseCompleteCallback },
		&listServiceCallback
	};

	do {
		S3_list_service(protocolG, accessKeyIdG, secretAccessKeyG, 0, 0,
						&listServiceHandler, &data);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (statusG == S3StatusOK) {
		if (!data.headerPrinted) {
			printListServiceHeader(allDetails);
		}
	}
	else {
		printError();
		return StatusResponseFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::list(const char * bucket, const int type)
{

	const char *bucketName = 0;
	if(bucket && strlen(bucket) != 0)
	{
		bucketName = bucket;
	}

	const char *prefix = 0, *marker = 0, *delimiter = 0;
	int maxkeys = 0, allDetails = 0;

	if (bucketName) {
		int ret = list_bucket(bucketName, prefix, marker, delimiter, maxkeys,
					allDetails, type);
		if(StatusOK != ret)
		{
			return ret;
		}

	}
	else {
		int ret = list_service(allDetails, type);
		if(StatusOK != ret)
		{
			return ret;
		}
	}
	return StatusOK;
}

int ceph::S3::list_bucket(const char *bucketName, const char *prefix,
						const char *marker, const char *delimiter,
						int maxkeys, int allDetails, const int type)
{
	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		bucketName,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	S3ListBucketHandler listBucketHandler =
	{
		{ &responsePropertiesCallback, &responseCompleteCallback },
		&listBucketCallback
	};

	list_bucket_callback_data data;

	snprintf(data.nextMarker, sizeof(data.nextMarker), "%s", marker);
	data.keyCount = 0;
	data.allDetails = allDetails;

	do {
		data.isTruncated = 0;
		do {
			S3_list_bucket(&bucketContext, prefix, data.nextMarker,
						   delimiter, maxkeys, 0, &listBucketHandler, &data);
		} while (S3_status_is_retryable((S3Status)statusG) && should_retry());
		if (statusG != S3StatusOK) {
			break;
		}
	} while (data.isTruncated && (!maxkeys || (data.keyCount < maxkeys)));

	if (statusG == S3StatusOK) {
		if (!data.keyCount) {
			printListBucketHeader(allDetails);
		}
	}
	else {
		printError();
		return StatusResponseFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::create_bucket(const char * bucket, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}

	const char *bucketName = bucket;

	if (!forceG && (S3_validate_bucket_name
					(bucketName, S3UriStyleVirtualHost) != S3StatusOK)) {
		_LOG_ERROR("\nWARNING: Bucket name is not valid for "
				"virtual-host style URI access.\n");
		_LOG_ERROR("Bucket not created.  Use -f option to force the "
				"bucket to be created despite\n");
		_LOG_ERROR("this warning.\n\n");
		return StatusInvalidArgument;
	}

	const char *locationConstraint = 0;
	S3CannedAcl cannedAcl = S3CannedAclPrivate;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3ResponseHandler responseHandler =
	{
		&responsePropertiesCallback, &responseCompleteCallback
	};

	do {
		S3_create_bucket(protocolG, accessKeyIdG, secretAccessKeyG,
						 0, bucketName, cannedAcl, locationConstraint, 0,
						 &responseHandler, 0);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (statusG == S3StatusOK) {
		_LOG_DEBUG("Bucket successfully created.\n");
	}
	else {
		printError();
		return StatusResponseFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::delete_bucket(const char * bucket, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}

	const char *bucketName = bucket;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3ResponseHandler responseHandler =
	{
		&responsePropertiesCallback, &responseCompleteCallback
	};

	do {
		S3_delete_bucket(protocolG, uriStyleG, accessKeyIdG, secretAccessKeyG,
						 0, bucketName, 0, &responseHandler, 0);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (statusG != S3StatusOK) {
		printError();
		return StatusResponseFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::test_bucket(const char * bucket, const int type)
{
	if (NULL == bucket || strlen(bucket) == 0) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}

	const char * bucketName = bucket;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3ResponseHandler responseHandler =
	{
		&responsePropertiesCallback, &responseCompleteCallback
	};

	char locationConstraint[64];
	do {
		S3_test_bucket(protocolG, uriStyleG, accessKeyIdG, secretAccessKeyG,
					   0, bucketName, sizeof(locationConstraint),
					   locationConstraint, 0, &responseHandler, 0);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	const char *result;

	switch (statusG) {
	case S3StatusOK:
		// bucket exists
		result = locationConstraint[0] ? locationConstraint : "USA";
		break;
	case S3StatusErrorNoSuchBucket:
		result = "Does Not Exist";
		break;
	case S3StatusErrorAccessDenied:
		result = "Access Denied";
		break;
	default:
		result = 0;
		break;
	}

	if (result) {
		_LOG_DEBUG("%-30s  %-20s\n" << bucketName << result);
	}
	else {
		printError();
		return StatusResponseFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::put_object(const char * file, const char * bucket, const char * key, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == key || 0 == strlen(key)) {
		_LOG_ERROR("\nERROR: Missing parameter: key\n");
		return StatusInvalidArgument;
	}
	if (NULL == file || 0 == strlen(file)) {
		_LOG_ERROR("\nERROR: Missing parameter: file\n");
		return StatusInvalidArgument;
	}

	const char *filename = file;
	uint64_t contentLength = 0;
	const char *cacheControl = 0, *contentType = 0, *md5 = 0;
	const char *contentDispositionFilename = 0, *contentEncoding = 0;
	int64_t expires = -1;
	S3CannedAcl cannedAcl = S3CannedAclPrivate;
	int metaPropertiesCount = 0;
	S3NameValue metaProperties[S3_MAX_METADATA_COUNT];
	int noStatus = 0;

	put_object_callback_data data;

	data.infile = 0;
	data.gb = 0;
	data.noStatus = noStatus;

	if (filename) {
		if (!contentLength) {
			struct stat statbuf;
			// Stat the file to get its length
			if (stat(filename, &statbuf) == -1) {
				_LOG_ERROR("\nERROR: Failed to stat file %s: " << filename);
				perror(0);
				return StatusFileObtainFailed;
			}
			contentLength = statbuf.st_size;
		}
		// Open the file
		if (!(data.infile = fopen(filename, "r" FOPEN_EXTRA_FLAGS))) {
			_LOG_ERROR("\nERROR: Failed to open input file %s: " << filename);
			perror(0);
			return StatusFileObtainFailed;
		}
	}
	else {
		// Read from stdin.  If contentLength is not provided, we have
		// to read it all in to get contentLength.
		if (!contentLength) {
			// Read all if stdin to get the data
			char buffer[64 * 1024];
			while (1) {
				int amtRead = fread(buffer, 1, sizeof(buffer), stdin);
				if (amtRead == 0) {
					break;
				}
				if (!growbuffer_append(&(data.gb), buffer, amtRead)) {
					_LOG_ERROR("\nERROR: Out of memory while reading "
							"stdin\n");
					return StatusOutOfMemory;
				}
				contentLength += amtRead;
				if (amtRead < (int) sizeof(buffer)) {
					break;
				}
			}
		}
		else {
			data.infile = stdin;
		}
	}

	data.contentLength = data.originalContentLength = contentLength;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		bucket,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	S3PutProperties putProperties =
	{
		contentType,
		md5,
		cacheControl,
		contentDispositionFilename,
		contentEncoding,
		expires,
		cannedAcl,
		metaPropertiesCount,
		metaProperties
	};

	S3PutObjectHandler putObjectHandler =
	{
		{ &responsePropertiesCallback, &responseCompleteCallback },
		&putObjectDataCallback
	};

	do {
		S3_put_object(&bucketContext, key, contentLength, &putProperties, 0,
					  &putObjectHandler, &data);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (data.infile) {
		fclose(data.infile);
	}
	else if (data.gb) {
		growbuffer_destroy(data.gb);
	}

	if (statusG != S3StatusOK) {
		printError();
		return StatusResponseFailed;
	}
	else if (data.contentLength) {
		_LOG_ERROR("\nERROR: Failed to read remaining %llu bytes from "
				"input\n" << (unsigned long long) data.contentLength);
		return StatusFileObtainFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::get_object(const char * file, const char * bucket, const char * _key, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == _key || 0 == strlen(_key)) {
		_LOG_ERROR("\nERROR: Missing parameter: key\n");
		return StatusInvalidArgument;
	}
	if (NULL == file || 0 == strlen(file)) {
		_LOG_ERROR("\nERROR: Missing parameter: file\n");
		return StatusInvalidArgument;
	}

	const char *bucketName = bucket;
	const char *key = _key;

	const char *filename = file;
	int64_t ifModifiedSince = -1, ifNotModifiedSince = -1;
	const char *ifMatch = 0, *ifNotMatch = 0;
	uint64_t startByte = 0, byteCount = 0;

	FILE *outfile = 0;

	if (filename) {
		// Stat the file, and if it doesn't exist, open it in w mode
		struct stat buf;
		if (stat(filename, &buf) == -1) {
			outfile = fopen(filename, "w" FOPEN_EXTRA_FLAGS);
		}
		else {
			// Open in r+ so that we don't truncate the file, just in case
			// there is an error and we write no bytes, we leave the file
			// unmodified
			outfile = fopen(filename, "r+" FOPEN_EXTRA_FLAGS);
		}

		if (!outfile) {
			_LOG_ERROR("\nERROR: Failed to open output file %s: " << filename);
			perror(0);
			return StatusFileObtainFailed;
		}
	}
	else {
		outfile = stdout;
	}

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		bucketName,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	S3GetConditions getConditions =
	{
		ifModifiedSince,
		ifNotModifiedSince,
		ifMatch,
		ifNotMatch
	};

	S3GetObjectHandler getObjectHandler =
	{
		{ &responsePropertiesCallback, &responseCompleteCallback },
		&getObjectDataCallback
	};

	do {
		S3_get_object(&bucketContext, key, &getConditions, startByte,
					  byteCount, 0, &getObjectHandler, outfile);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (statusG != S3StatusOK) {
		printError();
		return StatusResponseFailed;
	}

	fclose(outfile);

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::copy_object(const char * srcBucket, const char * srcKey, const char * dstBucket, const char * dstKey, const int type)
{
	if (NULL == srcBucket || 0 == strlen(srcBucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: source bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == srcKey || 0 == strlen(srcKey)) {
		_LOG_ERROR("\nERROR: Missing parameter: source key\n");
		return StatusInvalidArgument;
	}
	if (NULL == dstBucket || 0 == strlen(dstBucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: destination bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == dstKey || 0 == strlen(dstKey)) {
		_LOG_ERROR("\nERROR: Missing parameter: destination key\n");
		return StatusInvalidArgument;
	}

	const char *sourceBucketName = srcBucket;
	const char *sourceKey = srcKey;

	const char *destinationBucketName = dstBucket;
	const char *destinationKey = dstKey;

	const char *cacheControl = 0, *contentType = 0;
	const char *contentDispositionFilename = 0, *contentEncoding = 0;
	int64_t expires = -1;
	S3CannedAcl cannedAcl = S3CannedAclPrivate;
	int metaPropertiesCount = 0;
	S3NameValue metaProperties[S3_MAX_METADATA_COUNT];
	int anyPropertiesSet = 0;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		sourceBucketName,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	S3PutProperties putProperties =
	{
		contentType,
		0,
		cacheControl,
		contentDispositionFilename,
		contentEncoding,
		expires,
		cannedAcl,
		metaPropertiesCount,
		metaProperties
	};

	S3ResponseHandler responseHandler =
	{
		&responsePropertiesCallback,
		&responseCompleteCallback
	};

	int64_t lastModified;
	char eTag[256];

	do {
		S3_copy_object(&bucketContext, sourceKey, destinationBucketName,
					   destinationKey, anyPropertiesSet ? &putProperties : 0,
					   &lastModified, sizeof(eTag), eTag, 0,
					   &responseHandler, 0);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (statusG == S3StatusOK) {
		if (lastModified >= 0) {
			char timebuf[256];
			time_t t = (time_t) lastModified;
			strftime(timebuf, sizeof(timebuf), "%Y-%m-%dT%H:%M:%SZ",
					 gmtime(&t));
			_LOG_DEBUG("Last-Modified: %s\n" << timebuf);
		}
		if (eTag[0]) {
			_LOG_DEBUG("ETag: %s\n" << eTag);
		}
	}
	else {
		printError();
		return StatusResponseFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::delete_object(const char * bucket, const char * _key, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == _key || 0 == strlen(_key)) {
		_LOG_ERROR("\nERROR: Missing parameter: key\n");
		return StatusInvalidArgument;
	}

	const char *bucketName = bucket;
	const char *key = _key;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		bucketName,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	S3ResponseHandler responseHandler =
	{
		0,
		&responseCompleteCallback
	};

	do {
		S3_delete_object(&bucketContext, key, 0, &responseHandler, 0);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if ((statusG != S3StatusOK) &&
		(statusG != S3StatusErrorPreconditionFailed)) {
		printError();
		return StatusResponseFailed;
	}

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::set_acl(const char * file, const char * bucket, const char * _key, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == _key || 0 == strlen(_key)) {
		_LOG_ERROR("\nERROR: Missing parameter: key\n");
		return StatusInvalidArgument;
	}
	if (NULL == file || 0 == strlen(file)) {
		_LOG_ERROR("\nERROR: Missing parameter: file\n");
		return StatusInvalidArgument;
	}

	const char *bucketName = bucket;
	const char *key = _key;

	const char *filename = file;

	FILE *infile;

	if (filename) {
		if (!(infile = fopen(filename, "r" FOPEN_EXTRA_FLAGS))) {
			_LOG_ERROR("\nERROR: Failed to open input file %s: " << filename);
			perror(0);
			return StatusFileObtainFailed;
		}
	}
	else {
		infile = stdin;
	}

	// Read in the complete ACL
	char aclBuf[65536];
	aclBuf[fread(aclBuf, 1, sizeof(aclBuf), infile)] = 0;
	char ownerId[S3_MAX_GRANTEE_USER_ID_SIZE];
	char ownerDisplayName[S3_MAX_GRANTEE_DISPLAY_NAME_SIZE];

	// Parse it
	int aclGrantCount;
	S3AclGrant aclGrants[S3_MAX_ACL_GRANT_COUNT];
	if (!convert_simple_acl(aclBuf, ownerId, ownerDisplayName,
							&aclGrantCount, aclGrants)) {
		_LOG_ERROR("\nERROR: Failed to parse ACLs\n");
		fclose(infile);
		return StatusFileObtainFailed;
	}

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		bucketName,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	S3ResponseHandler responseHandler =
	{
		&responsePropertiesCallback,
		&responseCompleteCallback
	};

	do {
		S3_set_acl(&bucketContext, key, ownerId, ownerDisplayName,
				   aclGrantCount, aclGrants, 0, &responseHandler, 0);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (statusG != S3StatusOK) {
		printError();
		return StatusResponseFailed;
	}

	fclose(infile);

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::get_acl(const char * file, const char * bucket, const char * _key, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == _key || 0 == strlen(_key)) {
		_LOG_ERROR("\nERROR: Missing parameter: key\n");
		return StatusInvalidArgument;
	}
	if (NULL == file || 0 == strlen(file)) {
		_LOG_ERROR("\nERROR: Missing parameter: file\n");
		return StatusInvalidArgument;
	}

	const char *bucketName = bucket;
	const char *key = _key;

	const char *filename = file;

	FILE *outfile = 0;

	if (filename) {
		// Stat the file, and if it doesn't exist, open it in w mode
		struct stat buf;
		if (stat(filename, &buf) == -1) {
			outfile = fopen(filename, "w" FOPEN_EXTRA_FLAGS);
		}
		else {
			// Open in r+ so that we don't truncate the file, just in case
			// there is an error and we write no bytes, we leave the file
			// unmodified
			outfile = fopen(filename, "r+" FOPEN_EXTRA_FLAGS);
		}

		if (!outfile) {
			_LOG_ERROR("\nERROR: Failed to open output file %s: " << filename);
			perror(0);
			return StatusFileObtainFailed;
		}
	}
	else {
		outfile = stdout;
	}

	int aclGrantCount;
	S3AclGrant aclGrants[S3_MAX_ACL_GRANT_COUNT];
	char ownerId[S3_MAX_GRANTEE_USER_ID_SIZE];
	char ownerDisplayName[S3_MAX_GRANTEE_DISPLAY_NAME_SIZE];

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		bucketName,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	S3ResponseHandler responseHandler =
	{
		&responsePropertiesCallback,
		&responseCompleteCallback
	};

	do {
		S3_get_acl(&bucketContext, key, ownerId, ownerDisplayName,
				   &aclGrantCount, aclGrants, 0, &responseHandler, 0);
	} while (S3_status_is_retryable((S3Status)statusG) && should_retry());

	if (statusG == S3StatusOK) {
		fprintf(outfile, "OwnerID %s %s\n", ownerId, ownerDisplayName);
		fprintf(outfile, "%-6s  %-90s  %-12s\n", " Type",
				"                                   User Identifier",
				" Permission");
		fprintf(outfile, "------  "
				"------------------------------------------------------------"
				"------------------------------  ------------\n");
		int i;
		for (i = 0; i < aclGrantCount; i++) {
			S3AclGrant *grant = &(aclGrants[i]);
			const char *type;
			char composedId[S3_MAX_GRANTEE_USER_ID_SIZE +
							S3_MAX_GRANTEE_DISPLAY_NAME_SIZE + 16];
			const char *id;

			switch (grant->granteeType) {
			case S3GranteeTypeAmazonCustomerByEmail:
				type = "Email";
				id = grant->grantee.amazonCustomerByEmail.emailAddress;
				break;
			case S3GranteeTypeCanonicalUser:
				type = "UserID";
				snprintf(composedId, sizeof(composedId),
						 "%s (%s)", grant->grantee.canonicalUser.id,
						 grant->grantee.canonicalUser.displayName);
				id = composedId;
				break;
			case S3GranteeTypeAllAwsUsers:
				type = "Group";
				id = "Authenticated AWS Users";
				break;
			case S3GranteeTypeAllUsers:
				type = "Group";
				id = "All Users";
				break;
			default:
				type = "Group";
				id = "Log Delivery";
				break;
			}
			const char *perm;
			switch (grant->permission) {
			case S3PermissionRead:
				perm = "READ";
				break;
			case S3PermissionWrite:
				perm = "WRITE";
				break;
			case S3PermissionReadACP:
				perm = "READ_ACP";
				break;
			case S3PermissionWriteACP:
				perm = "WRITE_ACP";
				break;
			default:
				perm = "FULL_CONTROL";
				break;
			}
			fprintf(outfile, "%-6s  %-90s  %-12s\n", type, id, perm);
		}
	}
	else {
		printError();
		return StatusResponseFailed;
	}

	fclose(outfile);

	S3_deinitialize();
	return StatusOK;
}

int ceph::S3::generate_query_string(const char * bucket, string & url, const char * _key, const int type)
{
	if (NULL == bucket || 0 == strlen(bucket)) {
		_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}
	if (NULL == _key || 0 == strlen(_key)) {
		_key = 0;
	}

	const char *bucketName = bucket;
	const char *key = _key;

	int64_t expires = -1;
	const char *resource = 0;

	int ret = 0;
	if(StatusOK != (ret = S3_init(type)))
	{
		return ret;
	}

	S3BucketContext bucketContext =
	{
		0,
		bucketName,
		protocolG,
		uriStyleG,
		accessKeyIdG,
		secretAccessKeyG
	};

	char buffer[S3_MAX_AUTHENTICATED_QUERY_STRING_SIZE];

	S3Status status = S3_generate_authenticated_query_string
		(buffer, &bucketContext, key, expires, resource);

	if (status != S3StatusOK) {
		_LOG_ERROR("Failed to generate authenticated query string: %s\n" <<
			   S3_get_status_name(status));
		return StatusResponseFailed;
	}
	else {
		_LOG_DEBUG("%s\n" << buffer);
		url = buffer;
	}

	S3_deinitialize();
	return StatusOK;
}

bool ceph::S3::parseData(int type)
{
	if(!Config::rgwVec.size())
	{
		return false;
	}
	if(type < 0 || type > Config::rgwVec.size()-1)
	{
		return false;
	}
	user = Config::rgwVec[type]->user.c_str();
	host = Config::rgwVec[type]->host.c_str();
	accessKey = Config::rgwVec[type]->accessKey.c_str();
	secretKey = Config::rgwVec[type]->secretKey.c_str();
	return true;
}
