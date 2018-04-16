/*
 * bpm_tests.cpp
 *
 *  Created on: 2018年1月22日
 *      Author: bn
 */
#include <gtest/gtest.h>
#include "config.h"
#include <vector>
using namespace std;

class ConfigTest : public ::testing::Test
{
public:
	Config * config;

public:
  virtual void SetUp()
  {
	  config = Config::Instance();
  }

  virtual void TearDown()
  {

  }
};

TEST_F(ConfigTest, readRGWConfig_test)
{
	vector<string> vec1, vec2, vec3;
	vec1.push_back("SmallBucket");
	vec2.push_back("MiddleBucket");
	vec3.push_back("LargeBucket");
	Config::RGW rgw1 {
		"fxsf",
		"172.16.25.111",
		"2RSR05J8CKWEC974GCI5",
		"pyrcpTm3DCmIi3EMAnG6fOuJRP5K18Dk1oUg0pUi",
		vec1
	};
	Config::RGW rgw2 {
		"fxmf",
		"172.16.25.111",
		"MXWDEZRALCTN6NOOBJ7M",
		"vCGWgDyCxzMv8TQXxUqWLfclGsBvPbshqA3pWzf6",
		vec2
	};
	Config::RGW rgw3 {
		"fxlf",
		"172.16.25.111",
		"J6415GQPYXYOWFLE9YBG",
		"HyYhkp21a0gFqDcLUCFHcONbYw6sEAvG4JALwjhy",
		vec3
	};
	vector<Config::RGW> testVec ;
	testVec.push_back(rgw1);
	testVec.push_back(rgw2);
	testVec.push_back(rgw3);
	EXPECT_TRUE(Config::readRGWConfig()) << "ReadRGWConfig failed!" << endl;
	for(int i=0;i<3;i++)
	{
		EXPECT_STREQ(testVec[i].host.c_str(), Config::rgwVec[i]->host.c_str());
		EXPECT_STREQ(testVec[i].user.c_str(), Config::rgwVec[i]->user.c_str());
		EXPECT_STREQ(testVec[i].accessKey.c_str(), Config::rgwVec[i]->accessKey.c_str());
		EXPECT_STREQ(testVec[i].secretKey.c_str(), Config::rgwVec[i]->secretKey.c_str());
		EXPECT_STREQ(testVec[i].buckets[0].c_str(), Config::rgwVec[i]->buckets[0].c_str());
	}
}

TEST_F(ConfigTest, readJWTConfig_test)
{
	EXPECT_TRUE(config->readJWTConfig()) << "ReadJWTConfig failed!" << endl;
	EXPECT_STREQ("172.16.25.112", config->jwtcfg[0].ip.c_str());
	EXPECT_STREQ("8000", config->jwtcfg[0].port.c_str());
}

TEST_F(ConfigTest, readBSDRConfig_test)
{
	EXPECT_TRUE(config->ReadConfig("bsdrca.xml")) << "ReadBSDRConfig failed!" << endl;
}

TEST_F(ConfigTest, readPotConfig_test)
{
	EXPECT_TRUE(config->ReadPortConfig("bsdrport.xml")) << "ReadBSDRPortConfig failed!" << endl;
	EXPECT_STREQ("21000", config->portVec[0]->Attribute("port"));
	EXPECT_STREQ("21001", config->portVec[1]->Attribute("port"));
	EXPECT_STREQ("21002", config->portVec[2]->Attribute("port"));
}
