/*
 * bpm_tests.cpp
 *
 *  Created on: 2018年1月22日
 *      Author: bn
 */
#include <gtest/gtest.h>
#include "mybizpl.hpp"
using namespace std;
using namespace ndn;

class MyBizplTest : public ::testing::Test {

  public:
  	ndn::CMyBIZPL* instance;
    CQIHolder *holder;
protected:
  virtual void SetUp() {

    InitBnLog("BSDR", BN_LOG_MODULE_BSDR);
    BN_LOG_INTERFACE("TEST MYBIZPL start...");
    instance = ndn::CMyBIZPL::getInstance();
    holder = new CQIHolder;
    //加载
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
    //加载market表数据
    instance->uploadMarket(holder);

    instance->initMySQLPool();

  }

  virtual void TearDown() {

        Config::Instance()->portVec.clear();
        Config::Instance()->rgwVec.clear();
        Config::Instance()->dbvec.clear();
        ndn::CMyBIZPL::getInstance()->cqiHolderDestroy();

  }



inline  int writeJsonToCAuthToken(const ProducerData &input, CAuthUser&output){


		Json::Reader reader;
		Json::Value value;
		//解析方法
		if (reader.parse(input.param, value))
		{
			  output.tid = value["TID"].asString();
				output.virtualId = value["VirtualId"].asString();
				output.userPubKey = value["UserPubKey"].asString();
				output.ramdon = value["Random"].asString();
				output.sig = value["Signature"].asString();
				return 0;
		}
		
	return -1;
}


};




// //4.4


//新增market

// TEST_F(MyBizplTest, changeMarketBalance) {

//     CMarket cm;
//     cm.deviceGroupId="ztehhhh";
//     cm.market="bsdr2";
//     cm.tid="3";
//     cm.servFee="";
//     cm.balance=123456;
//     std::string res("");
//     instance->changeMarketBalance(cm,res);
//     cout<<res;
  

// }

// TEST_F(MyBizplTest, queryRegExist) {


//     CReg reg;
//     reg.authToken = "test1";
//     reg.blocklabel = "/China/Guangdong/Shenzhen";
//     reg.virtualId = "123";
//     reg.content = "123456";
//     reg.verification = "31";
//     reg.tid = "1";
//     reg.deviceGroupId = "ztehhhh";
//     reg.providerMarket = "bsdr2";
//     reg.verification = "31";
//     std::string res("");
//     instance->queryRegExist(reg,res);
//     cout<<res;

  

// }

// TEST_F(MyBizplTest, mainUpdateRegPrice) {

    
//     //加载market表数据
    
//     CReg reg;
//     reg.authToken = "test1";
//     reg.blocklabel = "/China/Guangdong/Shenzhen";
//     reg.virtualId = "123";
//     reg.tid = "1";
//     reg.price = "21223";
//     reg.deviceGroupId = "ztehhhh";
//     reg.providerMarket = "bsdr10";
//     reg.verification = "4";
//     std::string res("");
//     EXPECT_EQ(instance->mainUpdateRegPrice(reg,res),0);
//     cout<<res;


// }



// TEST_F(MyBizplTest, queryRegPrice) {

//     CReg reg;
//     reg.authToken = "test1";
//     reg.blocklabel = "/China/Guangdong/Shenzhen";
//     reg.virtualId = "123";
//     reg.content = "123456";
//     reg.verification = "31";
//     reg.tid = "1";
//     reg.deviceGroupId = "ztehhhh";
//     reg.providerMarket = "bsdr2";
//     std::string res("");
//     instance->queryRegPrice(reg,res);
//     cout<<res;


// }

// TEST_F(MyBizplTest, queryRegSum) {

//     CReg reg;
//     reg.authToken = "test1";
//     reg.virtualId = "123";
//     reg.blocklabel = "/China";
//     reg.content = "123456";
//     reg.verification = "31";
//     reg.tid = "1";
//     reg.price = "2";
//     reg.deviceGroupId = "ztehhhh";
//     reg.providerMarket = "bsdr2";
//     std::string res("");
//     instance->queryRegSum(reg,res);
//     cout<<res;

    

// }


// TEST_F(MyBizplTest, queryMarketFromMarket) {

//     CPay cpay;
//     cpay.authToken = "";
//     cpay.blocklabel = "/China/Guangdong/Shenzhen";
//     cpay.virtualId = "123";
//     cpay.verification = "31";
//     cpay.content = "123456";
//     cpay.consumerMarket = "bsdr2";
//     cpay.tid = "1";
//     cpay.deviceGroupId = "ztehhhh";
//     cpay.providerMarket = "bsdr10";
//     std::string res("");
//     instance->queryMarketFromMarket(cpay,res);
//     cout<<res;

// }


// TEST_F(MyBizplTest,queryMarketFromUser){

//     CPay cpay;
//     cpay.authToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJFYXJ0aGxlZGdlciBBVVRIIiwiaWF0IjoxNTIyNzM5NDEzLCJ1c2VybmFtZSI6IjAzIiwiZXhwIjoxNTIzMzQ0MjEzfQ.y1yOqEMK1VHO0s1rqs6xF3xSjCnTd2iL1nXlymbEhdI";
//     cpay.blocklabel = "/China/Guangdong/Shenzhen";
//     cpay.virtualId = "03";
//     cpay.verification = "31";
//     cpay.content = "123456";
//     cpay.consumerMarket = "bsdr10";
//     cpay.tid = "1";
//     cpay.deviceGroupId = "ztehhhh";
//     cpay.providerMarket = "bsdr10";
//     std::string res("");
//     instance->queryMarketFromUser(cpay,res);
//     cout<<res;

// }

// TEST_F(MyBizplTest,queryMarketFromOtherUser){

//     CPay cpay;
//     cpay.authToken = "test1";
//     cpay.blocklabel = "/China/Guangdong/Shenzhen";
//     cpay.virtualId = "123";
//     cpay.verification = "31";
//     cpay.content = "123456";
//     cpay.consumerMarket = "bsdr10";
//     cpay.tid = "1";
//     cpay.deviceGroupId = "ztehhhh";
//     cpay.providerMarket = "bsdr10";
//     std::string res("");
//     instance->queryMarketFromOtherUser(cpay,res);
//     cout<<res;

// }

// TEST_F(MyBizplTest,queryMarketFromTecCompany){
//    CPay cpay;
//     cpay.authToken = "test1";
//     cpay.blocklabel = "helloworld6";
//     cpay.virtualId = "123";
//     cpay.content = "123456";
//     cpay.consumerMarket = "bsdr0";
//       cpay.verification = "31";
//     cpay.tid = "1";
//     cpay.price = "2";
//     cpay.deviceGroupId = "ztehhhh";
//     cpay.providerMarket = "bsdr2";
//     std::string res("");
//     instance->queryMarketFromTecCompany(cpay,res);
//     cout<<res;


// }

// TEST_F(MyBizplTest,queryMarketFromTecCompanyAsUser){
//    CPay cpay;
//     cpay.authToken = "test1";
//     cpay.blocklabel = "helloworld6";
//     cpay.virtualId = "123";
//     cpay.content = "123456";
//     cpay.consumerMarket = "bsdr0";
//     cpay.tid = "1";
//     cpay.price = "2";
//     cpay.deviceGroupId = "ztehhhh";
//     cpay.providerMarket = "bsdr2";
//       cpay.verification = "31";
//     std::string res("");
//     instance->queryMarketFromTecCompanyAsUser(cpay,res);
//     cout<<res;

// }

// TEST_F(MyBizplTest,queryConsumeSum){
//     CPay cpay;
//     cpay.authToken = "test1";
//     cpay.blocklabel = "/China/Guangdong/Shenzhen";
//     cpay.virtualId = "123";
//     cpay.verification = "31";
//     cpay.content = "123456";
//     cpay.consumerMarket = "bsdr10";
//     cpay.tid = "1";
//     cpay.deviceGroupId = "ztehhhh";
//     cpay.providerMarket = "bsdr10";
//     std::string res("");
//     instance->queryConsumeSum(cpay,res);
//     cout<<res;

// }

//  TEST_F(MyBizplTest,queryConsumeBill){
  
//     CPay cpay;
//     cpay.authToken = "test1";
//     cpay.blocklabel = "/China/Guangdong/Shenzhen";
//     cpay.virtualId = "123";
//     cpay.verification = "31";
//     cpay.content = "123456";
//     cpay.consumerMarket = "bsdr10";
//     cpay.tid = "1";
//     cpay.deviceGroupId = "ztehhhh";
//     cpay.providerMarket = "bsdr10";
//     std::string res("");
//      instance->queryConsumeBill(cpay,res);
//      cout<<res;

// }


// TEST_F(MyBizplTest,queryMarketFromBn){

//     CMarket cm;
//     cm.market = "bsdr2";
//     cm.tid = "123";
//     cm.deviceGroupId = "ztehhhh";
//      // cm.verification = "31";
//     std::string res("");
//     instance->queryMarketFromBn(cm,res);
//     cout<<res;

// }


// TEST_F(MyBizplTest,reqPubkeySync){
//     CAuthUser user;
//     user.virtualId = "02";
//     user.tid = "09";
//     user.userPubKey = std::string("LS0tLS1CRUdJTiBQVUJMSUMgS0VZLS0tLS0KTUlHZk1BMEdDU3FHU0liM0RRRUJBUVVBQTRHTkFEQ0JpUUtCZ1FEV251aFdWWEFVMjlDU3BqRnA3K3hkM2toMgpzbkI0dWkrNFFkRjYyUFdIQjIzU25sL0VrU3hVeEFQbHlJR0FvaFZBOEptNWdES1NtV1ZoUkl5Q01EWlh3K3BlCnpzTHQ5cEcvYXE0WTR0NW5QNG1aVHo0K1dDNDEvTlJOYktGazd0WUdsUDVKNHoyOEcxejJTeUFOUHVpN0ZtbEkKTnZMY09JUmJiSkRKZnJjSFR3SURBUUFCCi0tLS0tRU5EIFBVQkxJQyBLRVktLS0tLQo=");
//     std::string res("");
//     instance->reqPubkeySync(user,res);
//     cout<<res;

// }

// TEST_F(MyBizplTest,getAuthRandom){
//     CAuthUser user;
//     user.virtualId = "02";
//     user.tid = "01";
//     std::string res("");
//     instance->getAuthRandom(user,res);
//     cout<<res;

//  }


TEST_F(MyBizplTest, getAuthTokenfromSig) {

    CAuthUser user;
    // user.virtualId = "01";
    // user.tid = "123";
    // user.sig = "qPfSqx+6X3LsXa6EMsvkZdcgbjCIC648pxiMbmpKe+kyBVnqErQiTdh7bAQ59ETWgjvdvfNVGqE1TuJn4zIFOmXUnlS2dduZDTqaZxJ7FP+Y0X7atVWtAd0DsybfCX0/7M1bA4MKbPmFCIgTkc0GLqYjAIGYoKXUgBraEzrtcPY=";
    std::string res("");
    ProducerData pd;
    pd.param = "{\"TID\":\"01\",\"VirtualId\":\"01\",\"Signature\":\"gZ6lzh1wPoXn4AeBMGz/7SwfNP3JKlTM4cTiAYip+uyEAGBHqQGr2yXmg8O7Gdq0+ZNsdyfYqoZaJVj+mEnavvRmVFOV1inDlyLn7xtTROSgwz3hssYaDprShw9SJCc9hLQVOaGWnc+8Qb6uhirBqLb8BaZXLF0CsYygJGd2U2k=\"}";
    
    writeJsonToCAuthToken(pd,user);
    instance->getAuthTokenfromSig(user,res);
    cout<<res;
}

