#include "producer.h"
#include "logging4.h"
//begin liulijin20170531 add
#include "json/myjson.hpp"
#include "mybizpl.hpp"
//end liulijin20170531 add
#include "algo/aes_algo.h"  //add by marty 2017/06/06

//begin cipher by liulijin add
#include "config.h"
//end cipher by liulijin add
#include "mywallet.h"

INIT_LOGGER("Producer");

static const int CONNECTION_RETRY_TIMER = 1;
static const int POS_TYPE_GETDATA = 7;
static const int POS_TYPE_GETPARM = 8;
// static const int POS_CONSUMER_MARKET = 8;
// static const int POS_DATA_MARKET = 9;
// static const string TYPE_GET_DATA = "getdata";
// static const string TYPE_GET_TRAN = "gettraninfo";
// //begin 20170824 liangyuanting
// static const string TYPE_GET_ALREADYENTER_COUNT = "getalreadyentercount";
// static const string TYPE_CET_WAITCONSUME_COUNT = "getwaitconsumecount";
// static const string TYPE_GET_TRAN_BY_PAGE = "gettraninfobypage";
// static const string TYPE_GET_TRAN_COUNT = "gettraninfocount";
//end 20170824 liangyuanting
Producer::Producer() : m_pface(nullptr), plistenThr(NULL), m_listenId(NULL), 
    m_shouldresume(true)
{
}

Producer::Producer(string brname, int pronum): m_pface(nullptr), 
    m_brname(brname),m_pronum(pronum), plistenThr(NULL),
    m_listenId(NULL), m_shouldresume(true)
{
}

Producer::~Producer()
{

}

bool Producer::run()
{
	plistenThr = new std::thread(&Producer::startService, this);

    if(NULL == plistenThr)
    {
        return false;
    }

    return true;
}

string Producer::getBrName()
{
	return m_brname;
}

int Producer::getProNum()
{
	return m_pronum;
}

void Producer::startService()
{
	bool shouldresume = true;

	string strName = "/yl/broadcast/bsdr/producer/" + Config::Instance()->m_deviceGroupId 
	               + "/" + m_brname + "/" + std::to_string(m_pronum);

	do
	{
		try
		{
			m_pface = std::unique_ptr<Face>(new Face);
			if(nullptr == m_pface)
			{
				_LOG_ERROR("producer new Face error.");
				continue;
			}

			// if(m_listenId != NULL)
			// {
			// 	m_face.unregisterPrefix(
			// 		m_listenId,
			// 		[]{},
			// 		[](const string& failInfo){}
			// 		);
				
			// 	m_listenId = NULL;
			// }

			_LOG_INFO("startService:" << strName);

			m_listenId = m_pface->setInterestFilter(
				strName,
				bind(&Producer::onInterest, this, _1, _2),
				RegisterPrefixSuccessCallback(),
				bind(&Producer::onRegisterFailed, this, _1, _2)
				);
			
			m_pface->processEvents();
		}
		catch (std::runtime_error& e) 
		{
			_LOG_ERROR("producer catch nfd error:" << e.what());
		}
		catch(...)
		{
			_LOG_ERROR("producer process events catch unknow error.");
		}

		m_pface = nullptr;

		if(m_shouldresume)
		{
			sleep(CONNECTION_RETRY_TIMER);
		}

	} while(m_shouldresume);
	
	_LOG_INFO("end service:" << strName);
}

void Producer::stop()
{
	_LOG_INFO("stop producer:[baar name:" << m_brname << "][producer number:" << m_pronum << "]");

	{
		std::lock_guard<std::mutex> lock(m_resumeMutex);
		m_shouldresume = false;
	}
	
	if(m_pface != nullptr)
	{
		m_pface->shutdown();
	}

	if(plistenThr != NULL)
	{
		plistenThr->join();
		plistenThr = NULL;
	}
}


//返回数据
void Producer::onInterest(const InterestFilter& filter, const Interest& interest)
{
  //begin michael 20170815 added for pay & ctxQuery
  std::lock_guard<std::mutex> lg(m_myself);
  //end michael 20170815 added
  _LOG_INFO("onInterest:" << interest);

	Name dataName(interest.getName());
    //
    std::string str1 = dataName.getSubName(POS_TYPE_GETPARM).toUri().erase(0,1);
	Name::Component gettypecom = dataName.get(POS_TYPE_GETDATA);
    std::string strGetType = gettypecom.toUri();
    std::string decodeUrl = ndn::CMyJSON::getInstance()->URLDecode(str1);
    if(strGetType == "getauthtoken")
    {
        _LOG_INFO("onInterest:" << decodeUrl);
        decodeUrl = decodeUrl.erase(0,1);
        decodeUrl = decodeUrl.erase(decodeUrl.size()-1,1);

    }
    
    ndn::ProducerData data1;
    data1.strOperType = strGetType;
    data1.param = decodeUrl;
   
    std::string content = "";
   
    BN_LOG_DEBUG("producer's param is %s",data1.param.c_str());
    ndn::CMyBIZPL::getInstance()->methodProcess(data1,content);

    //end 20170824 liangyuanting
	//begin encrypt the content by mali 20170606 add
    /* Encrypt the plaintext */  
    //128bits key.
    unsigned char   key[KEY_BYTE_NUMBERS]; 
    //Init vector.
    unsigned char   iv[KEY_BYTE_NUMBERS];

    /* A 128 bit IV */  
    memcpy(key, KEY, KEY_BYTE_NUMBERS);
    memcpy(iv, INITIAL_VECTOR, KEY_BYTE_NUMBERS);
    
    int  ciphertext_len=-1;
    int plaintext_len=-1;
  
    unsigned char *plaintextbuf = (unsigned char *)content.c_str();
    unsigned char *ciphertextbuf=NULL;

    if (Config::Instance()->m_sCipherSwitch == 1)
    {
        plaintext_len = strlen((const char*)plaintextbuf);
        ciphertextbuf=(unsigned char *)malloc(MAX_TEXT_SIZE*sizeof(char));
        try{
            ciphertext_len = encrypt(plaintextbuf, strlen((const char *)plaintextbuf), key, iv,
                                        ciphertextbuf);
        }
        catch (std::runtime_error& e) 
        {
            _LOG_ERROR("producer encrypt data error:" << e.what());
        }
        catch(...)
        {
            _LOG_ERROR("producer encrypt data catch unknow error.");
        }
    }

    // if (Config::Instance()->m_sCipherSwitch == 1)
    // {
    // hexdump(stdout,
    //     "=======plaintextbuf=======",
    //     plaintextbuf,
    //     plaintext_len);

    // hexdump(stdout,
    //     "=======ciphertextbuf=======",
    //     ciphertextbuf,
    //     ciphertext_len);
    // }
    //end encrypt the content by mali 20170606 add

    // Create Data packet
    shared_ptr<Data> data = make_shared<Data>();
  
    data->setName(dataName);
    //begin modified from 10 to 1 by liulijin 20170630
    data->setFreshnessPeriod(time::seconds(0));
    //end modified from 10 to 1 by liulijin 20170630

    //begin encrypt the content by mali 20170606 modify
    if (Config::Instance()->m_sCipherSwitch == 1)
    {
        data->setContent(reinterpret_cast<const uint8_t*>(ciphertextbuf), ciphertext_len);
        free(ciphertextbuf);
    }
    else
    {
        data->setContent(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());
    }
    //end encrypt the content by mali 20170606 modify

    // Sign Data packet with default identity
    m_keyChain.sign(*data);

    _LOG_DEBUG("D:" << *data);

    // Return Data packet to the requester
    if(m_pface != nullptr)
    {
    	m_pface->put(*data);
    }
    
}

void Producer::onRegisterFailed(const Name& profix, const std::string& reason)
{
	_LOG_ERROR("onRegisterFailed:" << reason);
	// m_shouldresume = false;

	if(nullptr != m_pface)
	{
		m_pface->shutdown();
	}
}
