// /*
//  * bpm_tests.cpp
//  *
//  *  Created on: 2018年1月22日
//  *      Author: bn
//  */
// #include <thread>
// #include <vector>
// #include <gtest/gtest.h>
// #include "bpm.h"
// #include "consumer.h"
// using namespace std;

// class BPMTest : public ::testing::Test
// {
// public:
// 	BPM* bpm;
// 	ndn::CQIHolder *holder;
// 	vector<PBlockData> vData;

// public:
//   virtual void SetUp()
//   {
// 	  bpm = BPM::Instance();
//   }

//   virtual void TearDown()
//   {

//   }
// };

// TEST_F(BPMTest, queue_put_test1)
// {
// 	ASSERT_TRUE(bpm != NULL) << "bpm object is not null!" << endl;
	
// 	for(int i=0;i<100;i++)
// 	{
// 		PMarketBlockData pdata = new MarketBlockData();
// 		pdata->func= PRODUCER_DATA_FUNC::MARKET_ATTACH_BNNODE;
		
// 		pdata->deviceGroupId=std::string("device00" + to_string(i));
// 		pdata->authToken=std::string("auth001" + to_string(i));
// 		pdata->devNo=std::string(""+ to_string(i));
// 		pdata->servfee=std::string("100" + to_string(i));
// 		pdata->market=std::string("bsdr" + to_string(i));
// 		BPM::Instance()->put((PBlockData)pdata);
// 	}
// 	 EXPECT_EQ(100, bpm->qSize()) << "queue data size is not correct!" << endl;
// }

// TEST_F(BPMTest, queue_get_test)
// {
// 	PMarketBlockData pdata = (PMarketBlockData)bpm->get();
// 	ASSERT_TRUE(pdata) << "pdata is null" << endl;
// 	EXPECT_EQ(5, pdata->func);
// 	EXPECT_STREQ("device000", pdata->deviceGroupId.c_str());
// 	EXPECT_STREQ("auth0010", pdata->authToken.c_str());
// 	EXPECT_STREQ("0", pdata->devNo.c_str());
// 	EXPECT_STREQ("1000", pdata->servfee.c_str());
// 	EXPECT_STREQ("bsdr0", pdata->market.c_str());

// 	EXPECT_EQ(99, bpm->qSize());
// 	thread thread1(&BPM::get, bpm);
// 	thread thread2(&BPM::get, bpm);
// 	thread thread3(&BPM::get, bpm);
// 	thread thread4(&BPM::get, bpm);
// 	thread thread5(&BPM::get, bpm);
// 	thread1.join();
// 	thread2.join();
// 	thread3.join();
// 	thread4.join();
// 	thread5.join();
// }


// TEST_F(BPMTest, queue_put_test2)
// {
// 	ASSERT_TRUE(bpm != NULL) << "bpm object is not null!";
// 	EXPECT_EQ(94, bpm->qSize());
// 	while(bpm->qSize())
// 	{
// 		bpm->get();
// 	}
// 	EXPECT_EQ(0, bpm->qSize());
// 	for(int i=0;i<100;i++)
// 	{
// 		PChainBlockData pdata = new ChainBlockData();
// 		pdata->func= PRODUCER_DATA_FUNC::REG_DATA_SET;
// 		pdata->virtualId = std::string("ssdsd");
// 		pdata->deviceGroupId=std::string("device00" + i);
// 		pdata->authToken=std::string("auth001" + i);
// 		pdata->devNo=std::string(""+i);
// 		pdata->s3srcFileName=std::string("file" + i);
// 		pdata->verification=std::string("verify" + i);
// 		pdata->servfee=std::string("100" + i);
// 		pdata->content=std::string("/content/" + i);
// 		pdata->market=std::string("bsdr" + i);
// 		pdata->price=std::string("0.1" + i);
// 		pdata->blocklabel=std::string("blocklabel" + i);
// 		vData.push_back(pdata);
			
// 		BPM::Instance()->put((PBlockData)pdata);
// 	}
// 	 EXPECT_EQ(100, bpm->qSize()) << "queue data size is not correct!";
// }

//  TEST_F(BPMTest, queue_put_multithread_test)
//  {
//  	ASSERT_TRUE(bpm != NULL) << "bpm object is not null!";
//  	for(int i=0;i<10;i++)
//  	{
//  		PChainBlockData pdata = new ChainBlockData();
//  		pdata->func=PRODUCER_DATA_FUNC::REG_DATA_SET;
//  		pdata->virtualId=std::string("sdasdsadas");
//  		pdata->deviceGroupId=std::string("device00" + i);
//  		pdata->authToken=std::string("auth001" + i);
//  		pdata->devNo=std::string(""+i);
//  		pdata->s3srcFileName=std::string("file" + i);
//  		pdata->verification=std::string("verify" + i);
//  		pdata->servfee=std::string("100" + i);
//  		pdata->content=std::string("/content/" + i);
//  		pdata->market=std::string("bsdr" + i);
//  		pdata->price=std::string("0.1" + i);
//  		pdata->blocklabel=std::string("blocklabel" + i);
//  		thread thread1(&BPM::put, bpm, pdata);
//  		thread thread2(&BPM::put, bpm, pdata);
//  		thread thread3(&BPM::put, bpm, pdata);
//  		thread thread4(&BPM::put, bpm, pdata);
//  		thread1.join();
//  		thread2.join();
//  		thread3.join();
//  		thread4.join();
//  	}
//  	EXPECT_EQ(140, bpm->qSize()) << "queue data size is not correct!" << endl;
//  }

// //TEST_F(BPMTest, doMarketMethod_test)
// //{
// //	holder = new ndn::CQIHolder;
// //	ndn::CMarket * cm = new ndn::CMarket();
// //	cm->deviceGroupId = "deviceGroupId";
// //	cm->balance = "balance";
// //	cm->marketPub = "marketPub";
// //	cm->market = "market";
// //	cm->marketPk = "marketPk";
// //	EXPECT_EQ(0, doMarketMethod(holder, cm, 0));
// //}
// TEST_F(BPMTest, doRegMethod_test)
// {

// }
// TEST_F(BPMTest, bizproc_test)
// {


// 	std::string input="{\"BlockLabel\": \"abc\", \
// 						\"ProviderMarket\": \"def\", \
// 						\"DeviceGroupId\": \"HIJ\", \
// 						\"ServFee\": \"123\", \
// 						\"S3SrcFileName\": \"uip12345677886654432344545567655443\", \
// 						\"S3SrcFileSize\": \"4000\", \
// 						\"Verification\": \"ASDJKS\", \
// 						\"Content\":{ \
// 						\"客户自定义\": \"SDJSD\", \
// 						\"SDJ\": \"ASDJIKSJD\"} \
// 					    }"; \
// 	cout<<input<<endl;
// 	Consumer cons;
// 	ConsumerData cd;
	
// 	cd.OperType=PRODUCER_DATA_FUNC::REG_DATA_SET;
// 	cd.JasonParam=input.c_str();

// }
// TEST_F(BPMTest, run_test)
// {

// }

