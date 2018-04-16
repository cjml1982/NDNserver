
#include "consumermgr.h"
#include "time.h"
#include "logging4.h"
#include "config.h"

INIT_LOGGER("ConsumerMgr");

static const int CONNECTION_RETRY_TIMER = 1;
static const int POS_NAME_BR_NAME_PUB   = 4;
static const int MONITOR_TIMER             = 10;
static const int MONITOR_WAIT_RESPONE_TIME = 30;

ConsumerMgr::ConsumerMgr()
{
	m_pface = nullptr;
	m_pthread = NULL;
	m_listenBrNameId = NULL;
	m_pthrMonitor = NULL;
}
ConsumerMgr::~ConsumerMgr()
{
	stop();
}

bool ConsumerMgr::run()
{
	BN_LOG_INTERFACE("run consumer manager...");

	m_shouldresume = true;

	if(m_pthread != NULL)
	{
		return true;
	}

	m_pthread = new std::thread(&ConsumerMgr::listenBrName, this);
	if(NULL == m_pthread)
	{
		BN_LOG_ERROR("run consumer manager error[new std::thread m_pthread fail]");
		return false;
	}

	if(NULL == m_pthrMonitor)
	{
		m_pthrMonitor = new std::thread(&ConsumerMgr::monitor, this);
	}
	if(NULL == m_pthrMonitor)
	{
		BN_LOG_ERROR("run consumer manager error[new std::thread m_pthrMonitor fail]");
		return false;
	}

	return true;
}

void ConsumerMgr::stop()
{
	BN_LOG_INTERFACE("stop all consumer...");

	{
		std::lock_guard<std::mutex> lock(m_resumeMutex);
		m_shouldresume = false;
	}

	if(m_pthrMonitor != NULL)
	{
		m_pthrMonitor->join();
		delete m_pthrMonitor;
		m_pthrMonitor = NULL;
	}

	
	if(m_pthread != NULL)
	{
		if(m_pface != nullptr)
		{
			m_pface->shutdown();
			m_pface = nullptr;
		}
		m_pthread->join();
		delete m_pthread;
		m_pthread = NULL;
	}
	
	{
		std::lock_guard<std::mutex> lck(m_vecConumerMtx);

		for(int i = 0; i < (int)m_vecConumer.size(); i++)
		{
			delete m_vecConumer[i];
		}

		m_vecConumer.clear();
	}

}

void ConsumerMgr::monitor()
{
	while(m_shouldresume)
	{
		time_t tm = ::time(NULL);

		//BN_LOG_INTERFACE("monitor[time:%s%s" , tm, "]");

		{
			std::lock_guard<std::mutex> lck(m_vecConumerMtx);
			for(int i = 0; i < (int)m_vecConumer.size();)
			{
				if((tm - m_vecConumer[i]->m_lastHeartbeat) > MONITOR_WAIT_RESPONE_TIME)
		    	{
		    	//	BN_LOG_INTERFACE("realese:%s" ,*(m_vecConumer[i]));
		    		m_vecConumer[i]->stop();
		    		delete m_vecConumer[i];

		    		m_vecConumer.erase(m_vecConumer.begin() + i);
		    		continue;
		    	}

				i++;
			}
		}

		sleep(MONITOR_TIMER);
	}
}

void ConsumerMgr::listenBrName()
{
	string strName("/yl/broadcast/baar/namepublish");

	do
	{
		try
		{
			if(nullptr == m_pface)
			{
				m_pface = std::unique_ptr<Face>(new Face);
				if(nullptr == m_pface)
				{
					continue;
				}
			}

			BN_LOG_DEBUG("consumer mgr listen:%s",strName.c_str());

			m_pface->setInterestFilter(strName,
				bind(&ConsumerMgr::onInterest, this, _1, _2),
				bind(&ConsumerMgr::onRegisterSuccess, this, _1),
				bind(&ConsumerMgr::onRegisterFailed, this, _1, _2));

			m_pface->processEvents();
		}
		catch (std::runtime_error& e)
		{
			BN_LOG_ERROR("ConsumerMgr catch nfd error:%s" , e.what());
		}
		catch(...)
		{
			BN_LOG_ERROR("conumermgr process events catch unknow error.");
		}

		m_pface = nullptr;

		if(m_shouldresume)
		{
			sleep(CONNECTION_RETRY_TIMER);
		}

	} while(m_shouldresume);

	_LOG_INFO("levea listenBrName.");
}

void ConsumerMgr::onInterest(const InterestFilter& filter, const Interest& interest)
{
	Name dataName(interest.getName());

	Name::Component comMacAddr = dataName.get(POS_NAME_BR_NAME_PUB);
    std::string strMacAddr = comMacAddr.toUri();

    string brId = strMacAddr;
    BN_LOG_DEBUG("onInterest, Timer[macaddr:%s%s",strMacAddr.c_str(),"]");

    time_t tm = ::time(NULL);

    int* pStatus = new int[Config::Instance()->m_cosumerThrNum + 1];
    if(NULL == pStatus)
    {
    	return;
    }
    for(int i = 0; i < Config::Instance()->m_cosumerThrNum + 1; i++)
    {
    	pStatus[i] = 0;
    }

    {
    	std::lock_guard<std::mutex> lck(m_vecConumerMtx);
	    for(int j = 0; j < (int)m_vecConumer.size(); )
	    {
	    	if(strMacAddr == m_vecConumer[j]->getBrName())
	    	{
	    		int iconmerNum = m_vecConumer[j]->getConumerNum();
	    		if(iconmerNum > Config::Instance()->m_cosumerThrNum)
	    		{
	    			_LOG_DEBUG("stop consumer[strMacAddr:" << strMacAddr << "][brname:"
			    		<< m_vecConumer[j]->getBrName()  
			    		<< "][consumerNum:" << m_vecConumer[j]->getConumerNum() << "]");

    			m_vecConumer[j]->stop();
    			m_vecConumer.erase(m_vecConumer.begin() + j);
    			continue;

    			}


    		m_vecConumer[j]->m_lastHeartbeat = tm;
    		pStatus[iconmerNum] = 1;
    		}

    		j++;
   		}

    	for(int k = 1; k < Config::Instance()->m_cosumerThrNum + 1; k++)
    	{
			if(0 == pStatus[k])
			{
				_LOG_INFO("start new consumer[macaddr:" << strMacAddr << "][k:" << k << "]");

				Consumer * pConsumer = new Consumer(strMacAddr, k);
				if(NULL == pConsumer)
				{
					_LOG_INFO("new consumer error,NULL point[macaddr:" << strMacAddr << "]");
					break;
				}

				if(!pConsumer->run())
				{
					delete pConsumer; pConsumer = NULL;
					break;
				}

    			m_vecConumer.push_back(pConsumer);
    		}
  		}
    }

    for(int i = 0; i < Config::Instance()->m_cosumerThrNum + 1; i++)
    {
    	pStatus[i] = 0;
    }

    delete[] pStatus; pStatus = NULL;

}

void ConsumerMgr::onRegisterSuccess(const Name& profix)
{
	BN_LOG_INTERFACE("register success[name:%s%s",profix.toUri().c_str(),"]");
}

void ConsumerMgr::onRegisterFailed(const Name& profix, const std::string& reason)
{
	_LOG_ERROR("onRegisterFailed:" << reason);
	if(m_pface != nullptr)
	{
		m_pface->shutdown();
	}
}
