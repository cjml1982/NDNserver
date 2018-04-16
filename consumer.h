#ifndef YL_BSDR_CONSUMER
#define YL_BSDR_CONSUMER

#include <ndn-cxx/face.hpp>

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <time.h>

#include <iostream>
#include "bpm.h"

using namespace std;
using namespace ndn;

struct ConsumerData
{
   unsigned int OperType;//取值为上面的PRODUCER_DATA_FUNC;
   string JasonParam; // Jason格式参数
} ;

class Consumer
{
public:
	Consumer();
	Consumer(const string& brname, int consumerNum);
	~Consumer();
public:
	bool run();
	void stop();

	string getBrName();
	int  getConumerNum();
private:
	void doGetData(int thrnum);
private:
	void onData(const Interest& interest, const Data& data);
	void onNack(const Interest& interest, const lp::Nack& nack);
	void onTimeout(const Interest& interest);

	inline void replace_all(std::string & s)  
				{  
					string::size_type pos = s.find("\"");  
					while(pos != std::string::npos){ // found   
						s.insert(pos,"\\");   
						pos = s.find("\"", pos + 2 );   
					}  
				}  
public:
	int writeJsonToChainData(const ConsumerData &input,ChainBlockData *output);
	//int writeJsonToMarketData(const ConsumerData &input,MarketBlockData *output);
public:
	volatile time_t m_lastHeartbeat;
private:
	string m_brname;       // 
	string m_market;

	int m_consumerNum;

	bool m_shouldResume;

	std::thread * pGetdataThr;


	long m_seqRcv;

public:
	friend std::ostream& operator<<(std::ostream& os, const Consumer& consumer);
};

// std::ostream& operator<<(std::ostream& os, const Consumer& consumer);

#endif	/* #ifndef YL_BSDR_CONSUMER */

