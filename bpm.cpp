
#include "bpm.h"

INIT_LOGGER("BPM");

BPM* BPM::pBPM = new BPM();

BPM::BPM()
{
}

BPM::~BPM()
{
	stop();
}

BPM* BPM::Instance()
{
    return pBPM;
}

void BPM::put(ChainBlockData *pdata)
{
	std::unique_lock<std::mutex> lock(m_queueMutex);
    m_queue.push(pdata);
    m_cond.notify_one();
}

size_t BPM::qSize()
{
	std::lock_guard<std::mutex> lock(m_queueMutex);
	return m_queue.size();
}

ChainBlockData* BPM::get()
{
	ChainBlockData* pRtn = NULL;
    std::unique_lock<std::mutex> lock(m_queueMutex);
    while(m_shouldResume && m_queue.empty())
    {
        m_cond.wait_for(lock, std::chrono::seconds(1));
    }

    if (!m_queue.empty())
    {
        pRtn = m_queue.front();
        m_queue.pop();
    }

    return pRtn;
}

bool BPM::run()
{
	BN_LOG_INTERFACE("run BPM...");

	m_shouldResume = true;
	int thrnum = Config::Instance()->m_bpmThrNum;

	for (int i = 0; i < thrnum; i++)
	{
		thread * pthr = new thread(&BPM::bizproc, this);
		if(NULL == pthr)
		{
			return false;
		}

		m_vecThrs.push_back(pthr);
	}

	return true;
}

void BPM::stop()
{
	m_shouldResume = false;

	std::vector<std::thread *>::iterator it = m_vecThrs.begin();
	for(; it != m_vecThrs.end(); ++it)
	{
		(*it)->join();
		delete (*it);
	}
	m_vecThrs.clear();

	while(!m_queue.empty())
    {
        PBlockData pdata = m_queue.front();
        delete(pdata);
        m_queue.pop();
    }
}

void BPM::bizproc()
{
	ChainBlockData *pdata = NULL;

	//这个就是线程函数了
	ndn::CQIHolder *holder =NULL;
	
	try{
		holder = new ndn::CQIHolder;
	}
	catch(std::bad_alloc &e)
	{
		_LOG_ERROR("new operator is failed");
		holder = NULL;
	}
	
	ndn::CMyWallet::getInstance()->initMysqlHolder(holder);
    
	while(m_shouldResume)  //
	{
		if((pdata = get()) != NULL)
		{

			ndn::CMyWallet::getInstance()->initMysqlHolder(holder);

			ndn::CReg reg = pdata->toCReg();

		    doRegMethod(holder,reg,pdata->func);
	
	     	//end liulijin20170531 add

			delete pdata;
			pdata = NULL;
		}

	}
}

//
class BPMFuncReg{
	public:
    int func;
    int(ndn::CMyWallet::*(proccess))(ndn::CQIHolder * holder, 
                            ndn::CReg &reg);
};






static BPMFuncReg regs[]={
	{PRODUCER_DATA_FUNC::REG_DATA_SET,&ndn::CMyWallet::mainRegSet},
	{PRODUCER_DATA_FUNC::REG_DATA_UPDATE,&ndn::CMyWallet::mainRegUpdate},
	{PRODUCER_DATA_FUNC::REG_DATA_DISABLE,&ndn::CMyWallet::mainRegBan}
	// {PRODUCER_DATA_FUNC::REG_DATA_PRICE_UPDATE,&ndn::CMyWallet::mainUpdateRegPrice}
};




int doRegMethod(ndn::CQIHolder * holder, ndn::CReg &reg,int func){

	int len = sizeof(regs)/sizeof(regs[0]);
	for(int i = 0;i<len;i++)
	{
		
		if(func == regs[i].func)
		{
			int result = (ndn::CMyWallet::getInstance()->*(regs[i].proccess))( holder,reg);
			if(result == -1)
			{
				BN_LOG_ERROR("doRegMethorError");
				return -1;
			}
		}
	}
	return 0;
}

