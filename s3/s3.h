/*
 * s3.h
 *
 *  Created on: 2018年2月26日
 *      Author: liaoyp
 */

#ifndef S3_S3_H_
#define S3_S3_H_

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "libs3.h"

// Some Windows stuff
#ifndef FOPEN_EXTRA_FLAGS
#define FOPEN_EXTRA_FLAGS ""
#endif

// Some Unix stuff (to work around Windows issues)
#ifndef SLEEP_UNITS_PER_SECOND
#define SLEEP_UNITS_PER_SECOND 1
#endif

static int forceG = 0;
static int showResponsePropertiesG = 0;
static S3Protocol protocolG = S3ProtocolHTTP;
static S3UriStyle uriStyleG = S3UriStylePath;
static int retriesG = 5;

static const char *accessKeyIdG = 0;
static const char *secretAccessKeyG = 0;

static int statusG = 0;
static char errorDetailsG[4096] = { 0 };
static char putenvBufG[256];

enum { StatusOK = 200, StatusInvalidArgument, StatusRequestFailed, StatusResponseFailed,
				StatusXmlParseFailure, StatusFileObtainFailed, StatusOutOfMemory, StatusWriteJsonFailed };

using namespace std;

namespace ceph {

typedef struct growbuffer
{
    // The total number of bytes, and the start byte
    int size;
    // The start byte
    int start;
    // The blocks
    char data[64 * 1024];
    struct growbuffer *prev, *next;
} growbuffer;

typedef struct list_service_data
{
    int headerPrinted;
    int allDetails;
} list_service_data;

typedef struct list_bucket_callback_data
{
    int isTruncated;
    char nextMarker[1024];
    int keyCount;
    int allDetails;
} list_bucket_callback_data;

typedef struct put_object_callback_data
{
    FILE *infile;
    growbuffer *gb;
    uint64_t contentLength, originalContentLength;
    int noStatus;
} put_object_callback_data;

class S3
{
private:
	const char * user;
	const char * host;
	const char * accessKey;
	const char * secretKey;

	static S3 * instance;


private:

	S3();
	~S3();

	void printError();
	uint64_t convertInt(const char *str, const char *paramName);
	int growbuffer_append(growbuffer **gb, const char *data, int dataLen);
	static void growbuffer_read(growbuffer **gb, int amt, int *amtReturn, char *buffer);
	void growbuffer_destroy(growbuffer *gb);
	int checkString(const char *str, const char *format);
	int64_t parseIso8601Time(const char *str);
	int convert_simple_acl(char *aclXml, char *ownerId,
	                              char *ownerDisplayName,
	                              int *aclGrantCountReturn,
	                              S3AclGrant *aclGrants);
	int should_retry();


	static S3Status responsePropertiesCallback
	    (const S3ResponseProperties *properties, void *callbackData);
	static void responseCompleteCallback(S3Status status,
	                                     const S3ErrorDetails *error,
	                                     void *callbackData);
	static S3Status listServiceCallback(const char *ownerId,
	                                    const char *ownerDisplayName,
	                                    const char *bucketName,
	                                    int64_t creationDate, void *callbackData);
	static S3Status listBucketCallback(int isTruncated, const char *nextMarker,
	                                   int contentsCount,
	                                   const S3ListBucketContent *contents,
	                                   int commonPrefixesCount,
	                                   const char **commonPrefixes,
	                                   void *callbackData);
	static int putObjectDataCallback(int bufferSize, char *buffer,
	                                 void *callbackData);
	static S3Status getObjectDataCallback(int bufferSize, const char *buffer,
	                                      void *callbackData);


	static void printListServiceHeader(int allDetails);
	static void printListBucketHeader(int allDetails);

	bool parseData(int type);

public:

	static S3 * getInstance();

	int S3_init(const int type = 0);

	int list_service(int allDetails, const int type = 0);
	int list(const char * bucket, const int type = 0);

	int list_bucket(const char *bucketName, const char *prefix,
	                        const char *marker, const char *delimiter,
	                        int maxkeys, int allDetails, const int type = 0);
	int create_bucket(const char * bucket, const int type = 0);
	int delete_bucket(const char * bucket, const int type = 0);
	int test_bucket(const char * bucket, const int type = 0);

	int put_object(const char * file, const char * bucket, const char * key, const int type = 0);
	int get_object(const char * file, const char * bucket, const char * key, const int type = 0);
	int copy_object(const char * srcBucket, const char * srcKey, const char * dstBucket, const char * dstKey, const int type = 0);
	int delete_object(const char * bucket, const char * key, const int type = 0);

	int set_acl(const char * file, const char * bucket, const char * key, const int type = 0);
	int get_acl(const char * file, const char * bucket, const char * key, const int type = 0);

	int generate_query_string(const char * bucket, string & url, const char * key = 0, const int type = 0);
};

}


#endif /* S3_S3_H_ */
