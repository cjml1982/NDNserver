/*
 * bpm_tests.cpp
 *
 *  Created on: 2018年1月22日
 *      Author: bn
 */
#include <gtest/gtest.h>
#include "mysql/mymysql.hpp"
using namespace std;

class MyMySQLTest : public ::testing::Test
{
public:
	ndn::CMyMYSQL * instance;

protected:
  virtual void SetUp()
  {
	  instance = ndn::CMyMYSQL::getInstance();
  }

  virtual void TearDown()
  {
  }


};

TEST_F(MyMySQLTest, getInstance_test)
{
	ASSERT_TRUE(NULL != instance);
}

TEST_F(MyMySQLTest, initMYSQL_test)
{
	EXPECT_TRUE( NULL == instance->getClient() );
	int ret = instance->initMYSQL();
	ASSERT_TRUE( NULL != instance->getClient() ) << "init mysql failed!" << endl;
	ASSERT_EQ(0, ret);
}
TEST_F(MyMySQLTest, connectMYSQL_test)
{
	const char* ip  = "172.16.25.112";
	const char* usr = "root";
	const char* pwd = "1qaz@WSX3edc$RFVyl";
	const char* db  = "mysql";
	ASSERT_EQ(0, instance->connectMYSQL(ip, usr, pwd, db)) << "connect mysql failed!" << endl;
	ASSERT_EQ(0,instance->closeMYSQL());
}

