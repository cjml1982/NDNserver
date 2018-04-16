/*
 * BsdrNamePub.cpp
 *
 *  Created on: 2017年6月29日
 *      Author:
 */

#include "bsdrnamepub.h"
#include "config.h"
#include "logging4.h"

INIT_LOGGER("BsdrNamePub");

BsdrNamePub::BsdrNamePub() 
{
	m_thr = NULL;

}

BsdrNamePub::~BsdrNamePub() 
{
    m_shouldResume = false;
	if(m_thr != NULL)
    {
        m_thr->join();
        delete m_thr;
        m_thr = NULL;
    }
}

bool BsdrNamePub::run()
{
	BN_LOG_INTERFACE("run bsdr name publisher...");

    m_shouldResume = true;

    m_thr = new std::thread(&BsdrNamePub::doRun, this);
    if(NULL == m_thr)
    {
        BN_LOG_ERROR("new bsdr name publisher thread error.");
        return false;
    }

	return true;
}

void BsdrNamePub::doRun()
{
    while(m_shouldResume)
    {
        Name nameprefix("/yl/broadcast/bsdr/namepublish");

        nameprefix.append(Config::Instance()->m_deviceGroupId.c_str());

        nameprefix.append(std::to_string(Config::Instance()->m_cosumerThrNum));
        Interest interest(nameprefix);
        interest.setInterestLifetime(time::milliseconds(1000));
        interest.setMustBeFresh(true);

        try
        {
            if(nullptr == m_pface)
            {
                m_pface = std::unique_ptr<Face>(new Face);
                if(nullptr == m_pface)
                {
                    BN_LOG_ERROR("br name publiser new Face error.");
                    continue;
                }
            }

            BN_LOG_INTERFACE("name publish:%s" ,nameprefix.toUri().c_str());

            m_pface->expressInterest(interest,
                bind(&BsdrNamePub::onData, this,  _1, _2),
                bind(&BsdrNamePub::onNack, this, _1, _2),
                bind(&BsdrNamePub::onTimeout, this, _1));

            m_pface->processEvents();
        }
        catch (std::runtime_error& e) 
        {
            BN_LOG_ERROR("bsdrname publisher catch nfd error:%s", e.what());
            m_pface = nullptr;
        }
        catch(...)
        {
            BN_LOG_ERROR("bsdrname publisher catch unknow error.");
            m_pface = nullptr;
        }

        if(m_shouldResume)
        {
            sleep(Config::Instance()->m_sBnrTimer);
        }
    }
}

void BsdrNamePub::stop()
{
    m_shouldResume = false;
    m_pface->shutdown();
}

void BsdrNamePub::onData(const Interest& interest, const Data& data)
{

}
void BsdrNamePub::onNack(const Interest& interest, const lp::Nack& nack)
{
    //_LOG_ERROR("br name publish on nack.");
}
void BsdrNamePub::onTimeout(const Interest& interest)
{
    //_LOG_ERROR("br name publish on timeout.");
}

