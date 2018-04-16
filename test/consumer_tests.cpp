// /*
//  * bpm_tests.cpp
//  *
//  *  Created on: 2018年1月22日
//  *      Author: bn
//  */
// #include "consumer.h"
// #include <gtest/gtest.h>
// using namespace std;

// class ConsumerTest : public ::testing::Test {
// protected:
//   virtual void SetUp() {

//   }

//   virtual void TearDown() {

//   }


// };

// TEST_F(ConsumerTest, transaction_test)
// {
// 	PChainBlockData data = new ChainBlockData();

	// std::string input="{\"Parameters\":{\"BlockLabel\": \"abc\", \
	// 					\"ProviderMarket\": \"def\", \
	// 					\"DeviceGroupId\": \"HIJ\", \
	// 					\"ServFee\": \"123\", \
	// 					\"S3SrcFile\": \"uip\", \
	// 					\"Verification\": \"ASDJKS\", \
	// 					\"Content\":{ \
	// 					\"客户自定义\": \"SDJSD\", \
	// 					\"SDJ\": \"ASDJIKSJD\"} \
	// 					}}"; \
// 	cout<<input<<endl;
// 	ConsumerData cd;

// 	cd.OperType=PRODUCER_DATA_FUNC::REG_DATA_SET;;
// 	cd.JasonParam=input.c_str();
// 	Consumer cons;
// 	cons.writeJsonToChainData(cd,data);
// 	cout<<data->toString()<<endl;
// }

// TEST_F(ConsumerTest, transaction_test2)
// {
// 	PMarketBlockData data = new MarketBlockData();

// 	std::string input="{\"Parameters\":{\"BlockLabel\": \"abc\", \
// 						\"ProviderMarket\": \"def\", \
// 						\"DeviceGroupId\": \"HIJ\", \
// 						\"ServFee\": \"123\", \
// 						\"S3SrcFile\": \"uip\", \
// 						\"Verification\": \"ASDJKS\", \
// 						\"Content\":{ \
// 						\"客户自定义\": \"SDJSD\", \
// 						\"SDJ\": \"ASDJIKSJD\"} \
// 						}}"; \
// 	cout<<input<<endl;
// 	Consumer cons;
// 	ConsumerData cd;

// 	cd.OperType=PRODUCER_DATA_FUNC::REG_DATA_SET;;
// 	cd.JasonParam=input.c_str();

// 	cons.writeJsonToMarketData(cd,data);
// 	cout<<data->toString()<<endl;
// }
