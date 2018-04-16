
#include "producermgr.h"
#include "config.h"
#include "logging4.h"

INIT_LOGGER("ProducerMgr");

static const int CONNECTION_RETRY_TIMER = 1;
static const int POS_NAME_BR_NAME_PUB   = 5;
static const int POS_CONSUMERNUM_BR_NAME_PUB = 6;

ProducerMgr::ProducerMgr() : m_pthread(NULL), m_pface(nullptr), 
    m_listenBrNameId(NULL), m_shouldresume(true)
{

}

ProducerMgr::~ProducerMgr()
{
	stop();
}

bool ProducerMgr::run()
{
	BN_LOG_INTERFACE("run producer manager...");

	m_shouldresume = true;

	if(m_pthread != NULL)
	{
		return true;
	}

	m_pthread = new std::thread(&ProducerMgr::listenBrName, this);
	if(NULL == m_pthread)
	{
		BN_LOG_ERROR("run producer manager error[new std::thread m_pthread fail]");
		return false;
	}

	return true;
}

void ProducerMgr::stop()
{
	_LOG_INFO("stop all producer...");

	std::vector<Producer *>::iterator itPro = m_vecPro.begin();
	for(; itPro != m_vecPro.end(); itPro++)
	{
		(*itPro)->stop();
	}

	_LOG_INFO("delete all producer...");

	itPro = m_vecPro.begin();
	for(; itPro != m_vecPro.end(); itPro++)
	{
		delete (*itPro);
	}
	m_vecPro.clear();

}

void ProducerMgr::listenBrName()
{
	string strName("/yl/broadcast/baar/publish/consumernum");
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

			BN_LOG_DEBUG("producer mgr listen:%s", strName.c_str());

			m_pface->setInterestFilter(strName,
				bind(&ProducerMgr::onInterest, this, _1, _2),
				bind(&ProducerMgr::onRegisterSuccess, this, _1), 
				bind(&ProducerMgr::onRegisterFailed, this, _1, _2));

			m_pface->processEvents();
		}
		catch (std::runtime_error& e)
		{
			BN_LOG_ERROR("ProducerMgr catch nfd error:%s",e.what());
		}
		catch(...)
		{
			BN_LOG_ERROR("producermgr process events catch unknow error.");
		}

		m_pface = nullptr;

		if(m_shouldresume)
		{
			sleep(CONNECTION_RETRY_TIMER);
		}

	} while(m_shouldresume);

	BN_LOG_INTERFACE("levea listenBrName.");
}

void ProducerMgr::onInterest(const InterestFilter& filter, const Interest& interest)
{
	Name dataName(interest.getName());

	Name::Component comMacAddr = dataName.get(POS_NAME_BR_NAME_PUB);
    std::string strMacAddr = comMacAddr.toUri();

    Name::Component comConsumerNum = dataName.get(POS_CONSUMERNUM_BR_NAME_PUB);
    std::string strConsumerNum = comConsumerNum.toUri();
    int iconsumernum = atoi(strConsumerNum.c_str());

    string brId = strMacAddr;
    // fprintf(stderr, "consumer ID[%s]\n", brId.c_str());
    BN_LOG_DEBUG("onInterest, Timer[macaddr:%s%s%d%s", strMacAddr.c_str() ,"][consumer num:", iconsumernum , "]");

    time_t tm = ::time(NULL);

    {
    	std::lock_guard<std::mutex> lck(m_brname2tMutex);
    	m_brname2t[brId] = tm;
    }

    {
    	std::lock_guard<std::mutex> lock(m_vecProMutex);

    	int count = 0;
	    vector<Producer *>::iterator iter = m_vecPro.begin();
	    while(iter != m_vecPro.end())
	    {
	    	if((*iter)->getBrName() == brId)
	    	{
	    		if((*iter)->getProNum() >= iconsumernum)
	    		{
	    			BN_LOG_DEBUG("stop producer[baar id:%s%s%d%s", brId.c_str() , "][producer number:" ,(*iter)->getProNum(),"]");
	    			(*iter)->stop();
	    			m_vecPro.erase(iter);
	    			continue;
	    		}

	    		count++;
	    	}

	    	++iter;
	    }

	    if(count < iconsumernum)
	    {
	    	for(; count < iconsumernum; count++)
	    	{
	    		BN_LOG_DEBUG("new producer[macaddr:%s%s%d", strMacAddr.c_str(), "][producer number:",count,"]");

		    	Producer * pProducer = new Producer(brId, count);
		    	if(NULL == pProducer)
		    	{
		    		BN_LOG_DEBUG("new producer error,NULL point.");
		    		break;
		    	}

		    	if(!pProducer->run())
		    	{
		    		delete pProducer;
		    		break;
		    	}
		    	else
		    	{
		    		m_vecPro.push_back(pProducer);
		    	}
	    	}
	    }
    }

}

void ProducerMgr::onRegisterSuccess(const Name& profix)
{
	_LOG_INFO("register success[name:" << profix << "]");
}

void ProducerMgr::onRegisterFailed(const Name& profix, const std::string& reason)
{
	_LOG_ERROR("onRegisterFailed:" << reason);
	if(m_pface != nullptr)
	{
		m_pface->shutdown();
	}
}

