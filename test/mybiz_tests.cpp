/*
 * bpm_tests.cpp
 *
 *  Created on: 2018年1月22日
 *      Author: bn
 */
#include <gtest/gtest.h>
#include "mybiz.hpp"
#include "mysql/mymysql.hpp"
#include "mysql/mymysqlpl.hpp"
#include "config.h"
using namespace std;

class MybizTest : public ::testing::Test
{
public:
	ndn::CMyBIZ * instance;

protected:
  virtual void SetUp()
  {
	  instance = ndn::CMyBIZ::getInstance();
  }

  virtual void TearDown()
  {
  }
};

TEST_F(MybizTest, initBIZLocal_test)
{
	ASSERT_TRUE(NULL != instance );
	string bsdrca = "bsdrca.xml";
	string bsdrport = "bsdrport.xml";
//	Config::Instance()->ReadConfig(bsdrca);
//	Config::Instance()->ReadPortConfig(bsdrport);
//	ndn::CMyMYSQLPL::getInstance()->initMainTLib();
//	ndn::CMyBIZ::getInstance()->createMySQLDB();
//	ndn::CMyBIZ::getInstance()->createMySQLDataTable();
//	ndn::CMyBIZ::getInstance()->createMySQLTXTable();
//	ndn::CMyBIZ::getInstance()->createMarketTable();
//	ndn::CMyBIZ::getInstance()->createTxLedgerTable();
//	EXPECT_EQ(0, instance->initBIZLocal());
}

TEST_F(MybizTest, getDBServerWithNoDB_test)
{

}
TEST_F(MybizTest, getDBServerWithDB_test)
{

//	EXPECT_TRUE(!ndn::CMyMYSQL::getInstance()->initMYSQL()) << "ReadPortConfig Failed";
//	EXPECT_EQ(3, Config::Instance()->dbvec.size());
//	EXPECT_TRUE(NULL != instance);
//	int ret = instance->getDBServerWithDB();
//	EXPECT_EQ(0, ret) << ret << endl;
}


TEST_F(MybizTest, getAddressesByAccountinitWalletHashMap_test)
{

}
