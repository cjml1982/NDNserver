/*
 * bpm_tests.cpp
 *
 *  Created on: 2018年1月22日
 *      Author: bn
 */
#include <gtest/gtest.h>
#include "s3file.h"
#include "config.h"
using namespace std;

class S3FileTest : public ::testing::Test
{
public:
	S3File s3file;
	CS3_Result_Parser * parser;
	CS3_Result s3Result;
public:
  virtual void SetUp()
  {
	  Config::readRGWConfig();
	  parser = CS3_Result_Parser::getInstance();
  }

  virtual void TearDown()
  {

  }
};

TEST_F(S3FileTest, putFile_test) {
	string file = "bsdrsvr";
	long size = 7*1024*1024;
	string result;
	EXPECT_EQ(StatusOK, s3file.putFile(file, size, result));
	file = "test/s3file_tests.cpp";
	size = 2858;
	EXPECT_EQ(StatusOK, s3file.putFile(file, size, result));
	EXPECT_EQ(0, parser->parseS3Blob(S3_ACTION_TYPE::PUT_FILE, result, s3Result));
	pair<string, string> s3access = s3Result.getS3Access();
	pair<string, string> s3bucket = s3Result.getS3Bucket();
	pair<string, string> s3dstfile = s3Result.getS3DstFile();
	pair<string, string> s3host = s3Result.getS3Host();
	pair<string, string> s3secret = s3Result.getS3Secret();
	pair<string, string> s3srcfile = s3Result.getS3SrcFile();
	pair<string, string> s3user = s3Result.getS3User();
	EXPECT_STREQ("S3Access", s3access.first.c_str());
	EXPECT_STREQ("2RSR05J8CKWEC974GCI5", s3access.second.c_str());
	EXPECT_STREQ("S3Bucket", s3bucket.first.c_str());
	EXPECT_STREQ("SmallBucket", s3bucket.second.c_str());
	EXPECT_STREQ("S3Host", s3host.first.c_str());
	EXPECT_STREQ("172.16.25.111", s3host.second.c_str());
	EXPECT_STREQ("S3User", s3user.first.c_str());
	EXPECT_STREQ("fxsf", s3user.second.c_str());
	EXPECT_STREQ("S3Secret", s3secret.first.c_str());
	EXPECT_STREQ("pyrcpTm3DCmIi3EMAnG6fOuJRP5K18Dk1oUg0pUi", s3secret.second.c_str());
	EXPECT_STREQ("S3DstFile", s3dstfile.first.c_str());
	EXPECT_TRUE(s3dstfile.second.size());
	result = "";
	EXPECT_DEATH(parser->parseS3Blob(S3_ACTION_TYPE::PUT_FILE, result, s3Result), "");
	EXPECT_EQ(StatusInvalidArgument, s3file.putFile("", size, result));
}

TEST_F(S3FileTest, getFile_test) {
	string dstfile = "20180309191118_s3file_tests.cpp";
	string bucket = "SmallBucket";
	string result;
	EXPECT_EQ(StatusOK, s3file.getFile(dstfile, bucket, result));
	EXPECT_EQ(0, parser->parseS3Blob(S3_ACTION_TYPE::GET_FILE, result, s3Result));
	EXPECT_EQ(StatusInvalidArgument, s3file.getFile("", bucket, result));
	EXPECT_EQ(StatusInvalidArgument, s3file.getFile(dstfile, "", result));
}

TEST_F(S3FileTest, updateFile_test) {
	string file = "test/s3file_tests.cpp";
	string dstfile = "s3file_tests20180302122820.cpp";
	string bukcet = "SmallBucket";
	long size = 2858;
	string result;
	EXPECT_EQ(StatusOK, s3file.updateFile(file, size, dstfile, bukcet, result));
	EXPECT_EQ(0, parser->parseS3Blob(S3_ACTION_TYPE::UPD_FILE, result, s3Result));
}

TEST_F(S3FileTest, deleteFile_test) {
	string file = "20b79f960120e04efa08e370cf76caa81020";
	string bucket = "SmallBucket";
	EXPECT_EQ(StatusOK, s3file.deleteFile(file, bucket));
}

