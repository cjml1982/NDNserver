/*
 * s3file.cpp
 *
 *  Created on: 2018年3月1日
 *      Author: liaoyp
 */

#include "s3file.h"
#include "config.h"
#include "logging4.h"
#include "json/myjson.hpp"
#include <ctime>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

INIT_LOGGER("s3file");

CS3_Result_Parser * CS3_Result_Parser::instance = new CS3_Result_Parser;

CS3_Result_Parser *
CS3_Result_Parser::getInstance()
{
  return instance;
}

int S3File::putFile(const string & srcfilename, const uint64_t size, string & result)
{
	if (!srcfilename.size()) {
		BN_LOG_ERROR("\nERROR: Missing parameter: file\n");
		return StatusInvalidArgument;
	}

	const char * bucket = 0;
	const char * key = 0;
	int type = 0;

	if(size > POSITION2)
	{
		type = LARGE;
	}else if (size > POSITION1 && size <= POSITION2)
	{
		type = MIDDLE;
	}else
	{
		type = SMALL;
	}

	bucket = Config::rgwVec[type]->buckets[0].c_str();

	time_t t;
	tm* local;
	char buf[128]= {0};
	t = time(NULL);
	local = localtime(&t);
	strftime(buf, 64, "%Y%m%d%H%M%S", local);
	string tmpfile = srcfilename;
	tmpfile.insert(0, "_").insert(0, buf);

	key = tmpfile.c_str();

	string host = Config::rgwVec[type]->host;
	string user = Config::rgwVec[type]->user;
	string access = Config::rgwVec[type]->accessKey;
	string secret = Config::rgwVec[type]->secretKey;
	string json;

	if(ndn::CMyJSON::getInstance()->writeJSONS3File(user, host, access, secret, bucket, key, json))
	{
		BN_LOG_ERROR("write json put file failed. ");
		return StatusWriteJsonFailed;
	}
	result = json;
	return StatusOK;
}

int S3File::getFile(const string & dstfile, const string & _bucket, string & result)
{
	if (!dstfile.size()) {
		BN_LOG_ERROR("\nERROR: Missing parameter: dstfile\n");
		return StatusInvalidArgument;
	}
	if (!_bucket.size()) {
		BN_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}

//	string hostpath = "/data/download/";
//	if(mkdir(hostpath.c_str(), 0766) == 0)
//	{
//		_LOG_INFO("make directory " << hostpath << " success.");
//	}

//	string tmpfile = dstfile;
//	string file = tmpfile.replace(0, 15, "");
//	const char * filename = hostpath.append(file).c_str();
	const char * bucket = _bucket.c_str();
	const char * key = dstfile.c_str();
	int type = 0;

	if(!strcmp(bucket, SB.c_str()))
	{
		type = SMALL;
	}else if (!strcmp(bucket, MB.c_str()))
	{
		type = MIDDLE;
	}else if (!strcmp(bucket, LB.c_str()))
	{
		type = LARGE;
	}else
	{
		BN_LOG_ERROR("cannot find the bucket: ", bucket);
		return StatusInvalidArgument;
	}

//	int ret = 0;
//	if(StatusOK != (ret = ceph::S3::getInstance()->get_object(filename, bucket, key, type)))
//	{
//		_LOG_ERROR("get object failed.");
//		return ret;
//	}

	string host = Config::rgwVec[type]->host;
	string user = Config::rgwVec[type]->user;
	string access = Config::rgwVec[type]->accessKey;
	string secret = Config::rgwVec[type]->secretKey;
	string json;
	if(ndn::CMyJSON::getInstance()->writeJSONS3File(user, host, access, secret, bucket, key, json))
	{
		BN_LOG_ERROR("write json get file failed. ");
		return StatusWriteJsonFailed;
	}
	result = json;

	return StatusOK;
}

int S3File::deleteFile(const string & dstfile, const string & _bucket)
{
	if (!dstfile.size()) {
		BN_LOG_ERROR("\nERROR: Missing parameter: dstfile\n");
		return StatusInvalidArgument;
	}
	if (!_bucket.size()) {
		BN_LOG_ERROR("\nERROR: Missing parameter: bucket\n");
		return StatusInvalidArgument;
	}

	const char * key = dstfile.c_str();
	const char * bucket = _bucket.c_str();
	int type = 0;

	if(!strcmp(bucket, SB.c_str()))
	{
		type = SMALL;
	}else if (!strcmp(bucket, MB.c_str()))
	{
		type = MIDDLE;
	}else if (!strcmp(bucket, LB.c_str()))
	{
		type = LARGE;
	}else
	{
		BN_LOG_ERROR("cannot find the bucket: ", bucket);
		return StatusInvalidArgument;
	}

	int ret = 0;
	if(StatusOK != (ret = ceph::S3::getInstance()->delete_object(bucket, key, type)))
	{
		BN_LOG_ERROR("delete object failed.");
		return ret;
	}

	return StatusOK;
}
int S3File::updateFile(const string & file, const uint64_t size, const string & dstfile, const string & bucket, string & result)
{
	int ret = 0;
	if(StatusOK != (ret = deleteFile(dstfile, bucket))){
		BN_LOG_ERROR("update file failed.");
		return ret;
	}
	if(StatusOK != (ret = putFile(file, size, result))){
		BN_LOG_ERROR("update file failed.");
		return ret;
	}
	return StatusOK;
}




