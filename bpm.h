#ifndef YL_BSDR_BUSINESS_PROCESS_MANAGER
#define YL_BSDR_BUSINESS_PROCESS_MANAGER



//begin liulijin20170531 add
#include "mybizpl.hpp"
//end liulijin20170531 add
#include "mywallet.h"

using namespace std;

typedef class tagBlockData
{
    
    public:
    std::string authToken;
    std::string devNo;
    std::string timeStamp;
    std::string market;
    std::string deviceGroupId;
    std::string servfee;
    int func;


   
}  BlockData, *PBlockData;


//struct ConsumerData
//{
//	std::string param;
//	unsigned int operType;
//};
typedef class ChainBlockData:public tagBlockData
{
    public:
    std::string virtualId;
    std::string content;
    std::string blocklabel;
    std::string market;
    std::string tid;
    std::string deviceGroupId;
    std::string price;
    std::string s3srcFileName;
    std::string s3srcFileSize;
    std::string verification;
    std::string authtoken;

    ndn::CReg toCReg()
    {
        ndn::CReg reg;
        reg.virtualId = virtualId;
        reg.blocklabel = blocklabel;
        reg.tid = tid;
        reg.content = content;
        reg.providerMarket = market;
        reg.deviceGroupId = deviceGroupId;
        reg.s3srcFileName = s3srcFileName;
        reg.s3srcFileSize = atol(s3srcFileSize.c_str());
        reg.price = price;
        reg.servfee = servfee;
        reg.verification = verification;
        reg.authToken = authtoken;
        
        return reg;
    }

    string toString()
    {
    	return "[devNo:" + devNo + "][timeStamp:" + timeStamp
    	     + "][content:" + content + "][blocklabel:" + blocklabel
             + "][virtualId:"+virtualId  
             + "][authToken:" + authToken 
    	     + "][market:" + market + "][price:" + price
             + "][deviceGroupId:" + deviceGroupId + "]"+"][servfee:" + servfee 
             + "][verification:" + verification + "]";
    }
}  ChainBlockData,*PChainBlockData;

// typedef class MarketBlockData:public tagBlockData
// {
//     public:

//     std::string balance;
   
//     string toString()
//     {
//     	return "[devNo:" + devNo + "][timeStamp:" + timeStamp
//              + "][authToken:" + authToken
//     	     + "][market:" + market
//              + "][deviceGroupId:" + deviceGroupId + "]"+"][servfee:" + servfee
//              + "][balance:" + balance + "]";
//     }
// }  MarketBlockData,*PMarketBlockData;

int doRegMethod(ndn::CQIHolder * holder, ndn::CReg &reg,int str);
//int doMarketMethod(ndn::CQIHolder * holder, ndn::CMarket *cm,int func);

/*
*  BPM:business process manager
*/
class BPM
{
protected:
	BPM();
	~BPM();
public:
	static BPM* pBPM;
public:
	static BPM* Instance();

public:
	bool run();
	void stop();
	void put(ChainBlockData * pdata);
	ChainBlockData* get();
	size_t qSize();

private:
	void bizproc();   //business process

private:
	volatile bool m_shouldResume;

	vector<thread *> m_vecThrs;

	std::mutex m_queueMutex;
    std::condition_variable m_cond; 
    std::queue<ChainBlockData*> m_queue;

};






#endif	/* #ifndef YL_BSDR_BUSINESS_PROCESS_MANAGER */
