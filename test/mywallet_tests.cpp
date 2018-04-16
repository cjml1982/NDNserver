/*
 * bpm_tests.cpp
 *
 *  Created on: 2018年1月22日
 *      Author: bn
 */
#include <gtest/gtest.h>
#include "mywallet.h"
#include <unistd.h>
using namespace std;
using namespace ndn;
class MyWalletTest : public ::testing::Test
{
public:
 ndn::CMyWallet* instance;
 CQIHolder *holder ;


protected:


  static void SetUpTestCase() {
  
 
  }



  static void TearDownTestCase() {
 
  }



  virtual void SetUp() {
	     //加载
    InitBnLog("BSDR", BN_LOG_MODULE_BSDR);
    BN_LOG_INTERFACE("TEST MYWALLET start...");
  	holder = new CQIHolder;
		instance = ndn::CMyWallet::getInstance();;
		string s1 = string("");
		s1.append(__XMLFILE);
		string s2 = string("");
		s2.append(_XMLFILE_PORT);

		Config::Instance()->ReadConfig(s1);
		Config::Instance()->ReadPortConfig(s2);
		Config::readRGWConfig();
    Config::Instance()->readJWTConfig();
    ndn::CMyBIZ::getInstance()->initMainTLib();
		instance->initWalletHashMap(s1);
		instance->initMysqlHolder(holder);
		
    instance->initMySQLPool();
    instance->reChain();
	

    


  }

  virtual void TearDown() {
    // //每一次内部容器清空
    Config::Instance()->portVec.clear();
    Config::Instance()->rgwVec.clear();
    Config::Instance()->dbvec.clear();
    //market为map 不需要清空
 

  }





};



//第一个 插入market







// // // //异步的情况



// TEST_F(MyWalletTest, getVerification){

// string input1 = "/China/Guangdong/Shenzhen/1053";
// string input2 = "";
// string output;
// instance->getVerif(input1,input2,output);
// cout<<output;

// }

 
// TEST_F(MyWalletTest, mainRegSet) {

    
//     //加载market表数据

//     string input1 = "123456";
//     string input2 = "";
//     string output;
//     instance->getVerif(input1,input2,output);
//     cout<<output;
    
//     CReg reg;
//     reg.authToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJFYXJ0aGxlZGdlciBBVVRIIiwiaWF0IjoxNTIyNzM5NDEzLCJ1c2VybmFtZSI6IjAzIiwiZXhwIjoxNTIzMzQ0MjEzfQ.y1yOqEMK1VHO0s1rqs6xF3xSjCnTd2iL1nXlymbEhdI";
//     reg.blocklabel = "/China/Guangdong/Shenzhen/104/";
//     reg.virtualId = "03";
//     reg.content = "123456";
//     // reg.s3srcFileName = "whatthehelloworld";
//     // reg.s3srcFileSize = 10;
//     reg.tid = "1";
//     reg.price = "21";
//     reg.deviceGroupId = "ztehhhh";
//     reg.providerMarket = "bsdr2";
//     reg.verification = output;
    
  
//     instance->mainRegSet(holder,reg);
//     instance->readChainQueueElement();
// }

TEST_F(MyWalletTest, mainRegBan) {

   
    string input1 = "123456";
    string input2 = "";
    string output;
    instance->getVerif(input1,input2,output);
    cout<<output;
    
    CReg reg;
    reg.authToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJFYXJ0aGxlZGdlciBBVVRIIiwiaWF0IjoxNTIyNzM5NDEzLCJ1c2VybmFtZSI6IjAzIiwiZXhwIjoxNTIzMzQ0MjEzfQ.y1yOqEMK1VHO0s1rqs6xF3xSjCnTd2iL1nXlymbEhdI";
    reg.blocklabel = "/China/Guangdong/Shenzhen/104/";
    reg.virtualId = "03";
    reg.content = "123456";
    // reg.s3srcFileName = "whatthehelloworld";
    // reg.s3srcFileSize = 10;
    reg.tid = "1";
    reg.price = "21";
    reg.deviceGroupId = "ztehhhh";
    reg.providerMarket = "bsdr2";
    reg.verification = output;
    
  
    
    EXPECT_EQ(instance->mainRegBan(holder,reg),0);
    instance->readChainQueueElement();
}

// TEST_F(MyWalletTest, mainRegSet1) {

    
//     //加载market表数据
    
//     CReg reg;
//     reg.authToken = "test1";
//     reg.blocklabel = "he22";
//     reg.virtualId = "123";
//     reg.content = "123456";
//     reg.s3srcFileName = "whatthehelloworld";
//     reg.s3srcFileSize = 10;
//     reg.tid = "1";
//     reg.price = "21";
//     reg.deviceGroupId = "ztehhhh";
//     reg.providerMarket = "bsdr2";
//     reg.verification = "1";
//     //reg.checkAddOrUpdate();
  
//     instance->mainRegSet(holder,reg);

// }

// TEST_F(MyWalletTest, mainRegUpdate) {

//     CReg reg;
//     reg.authToken = "test1";
//     reg.blocklabel = "/China/Guangdong/Shenzhen/1";
//     reg.virtualId = "123";
//     reg.content = "helloworld";
//     reg.s3srcFileName = "whatthehelloworld";
//     reg.s3srcFileSize = 10;
//     reg.tid = "1";
//     reg.price = "2300";
//     reg.deviceGroupId = "ztehhhh";
//     reg.providerMarket = "bsdr10";
//     reg.verification = "1";
//     instance->mainRegUpdate(holder,reg);
//     instance->readChainQueueElement();
//  }










// TEST_F(MyWalletTest, deleteMarketInfo) {

  // CMarket *cm = new CMarket();
  // cm->market = "bsdr1";
  // cm->balance = 1232323;
  // cm->tid = "2132132";
  // cm->deviceGroupId = "ztehhhh";
//   EXPECT_EQ(instance->deleteMarketInfo(holder,cm),0);
  
// }

// TEST_F(MyWalletTest, initWalletHashMap_test) {
// 	EXPECT_EQ(0, instance->initWalletHashMap("bsdrca.xml"));

// }
//
//TEST_F(MyWalletTest, getXMLVec_test)
//{
//
//}
//TEST_F(MyWalletTest, getMyLocalName_test)
//{
//
//}
//TEST_F(MyWalletTest, initMySQLPool_test)
//{
//	int size = 0;
//	EXPECT_EQ(0, instance->initMySQLPool()) << "initMySQLPool call faild" << endl ;
//	EXPECT_EQ(0, ndn::CMyInstQueue::getInstance()->qGetSize(size)) << "get pool size faild" << endl;
//	EXPECT_NE(0, size) << "pool size is zero" << endl;
//}
//TEST_F(MyWalletTest, doRun_test)
//{
//
//}

//TEST_F(MyWalletTest, setMapKeyValue_test)
//{
//
//}
//TEST_F(MyWalletTest, getMapValue_test)
//{
//
//}
//TEST_F(MyWalletTest, writeQueueElement_test)
//{
//
//}
//TEST_F(MyWalletTest, readQueueElement_test)
//{
//
//}
//TEST_F(MyWalletTest, runReader_test)
//{
//
//}
//TEST_F(MyWalletTest, reChain_test)
//{
//
//}
//TEST_F(MyWalletTest, initMysqlHolder_test)
//{
//
//}
//TEST_F(MyWalletTest, mainRegtest_test)
//{
//
//}
//TEST_F(MyWalletTest, checkInputString_test)
//{
//
//}

// TEST_F(MyWalletTest, reChain){
//   instance->reChain();
//   instance->readChainQueueElement();
// }




