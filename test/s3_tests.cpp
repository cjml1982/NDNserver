 /*
  * bpm_tests.cpp
  *
  *  Created on: 2018年1月22日
  *      Author: bn
  */
 #include <gtest/gtest.h>
 #include "s3/s3.h"
 #include "config.h"

 using namespace std;
 using namespace ceph;

 class S3Test : public ::testing::Test
 {
 public:
 	S3 * s3;
 	enum TYPE {SMALL, MIDDLE, LARGE, NONE};

 public:
   virtual void SetUp()
   {
 	  s3 = S3::getInstance();
 	  Config::readRGWConfig();
   }

   virtual void TearDown()
   {

   }
 };


 TEST_F(S3Test, init_user_test) {
 	ASSERT_EQ(StatusOK, s3->S3_init()) << "Failed to initialize libs3!" << endl;
 	ASSERT_EQ(StatusOK, s3->S3_init(MIDDLE)) << "Failed to initialize libs3!" << endl;
 	ASSERT_EQ(StatusOK, s3->S3_init(LARGE)) << "Failed to initialize libs3!" << endl;
 	ASSERT_EQ(StatusXmlParseFailure, s3->S3_init(NONE)) << "Failed to initialize libs3!" << endl;
 }

 TEST_F(S3Test, create_bucket_test) {
 	EXPECT_EQ(StatusOK,s3->create_bucket("test"));
 //	EXPECT_EQ(StatusOK,s3->create_bucket("TestSmallBucket", SMALL));
 //	EXPECT_EQ(StatusOK,s3->create_bucket("TestMiddleBucket", MIDDLE));
 //	EXPECT_EQ(StatusOK,s3->create_bucket("TestLargeBucket", LARGE));
 }

 TEST_F(S3Test, test_bucket_test) {
 	EXPECT_EQ(StatusOK,s3->test_bucket("SmallBucket"));
 	EXPECT_EQ(StatusOK,s3->test_bucket("MiddleBucket"));
 	EXPECT_EQ(StatusOK,s3->test_bucket("LargeBucket"));
 	EXPECT_EQ(StatusOK,s3->test_bucket("none"));
 }


 TEST_F(S3Test, delete_bucket_test) {
 	EXPECT_EQ(StatusOK,s3->delete_bucket("test"));
 }

 TEST_F(S3Test, put_object_test) {
 	EXPECT_EQ(StatusOK,s3->put_object("test/s3_tests.cpp", "TestSmallBucket", "test_cpp_001"));
 }

 TEST_F(S3Test, get_object_test) {
 	EXPECT_EQ(StatusOK,s3->get_object("/home/bn/test_cpp_001", "TestSmallBucket", "test_cpp_001"));
 }
 TEST_F(S3Test, copy_object_test) {
 	EXPECT_EQ(StatusOK,s3->copy_object("TestSmallBucket", "test_cpp_001", "SmallBucket", "test_cpp_001"));
 }

 TEST_F(S3Test, generate_query_string_test) {
 	string url;
 	EXPECT_EQ(StatusOK,s3->generate_query_string("TestSmallBucket", url, 0, SMALL));
 	EXPECT_EQ(StatusOK,s3->generate_query_string("SmallBucket", url, "20180313112057_bsdrsvr", SMALL));
 }

 TEST_F(S3Test, delete_object_test) {
 	EXPECT_EQ(StatusOK,s3->delete_object("TestSmallBucket", "test_cpp_001"));
 	EXPECT_EQ(StatusOK,s3->delete_object("SmallBucket", "test_cpp_001"));
 }

 TEST_F(S3Test, list_test) {
 	EXPECT_EQ(StatusOK,s3->list(""));
 	EXPECT_EQ(StatusOK,s3->list("SmallBucket"));
 	EXPECT_EQ(StatusOK,s3->list("", MIDDLE));
 	EXPECT_EQ(StatusOK,s3->list("MiddleBucket", MIDDLE));
 	EXPECT_EQ(StatusOK,s3->list("", LARGE));
 	EXPECT_EQ(StatusOK,s3->list("LargeBucket", LARGE));
 	EXPECT_EQ(StatusResponseFailed,s3->list("None", LARGE));
 }

