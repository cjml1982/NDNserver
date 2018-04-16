
#include "consumer.h"
#include "config.h"
#include "tinyxml/tinyxml.h"

#include "logging4.h"
#include <sstream>
#include "algo/aes_algo.h"  //add by marty 2017/06/06

INIT_LOGGER("Consumer");
static const string XML_ITEMNAME_PARAM = "param";
static const string XML_ITEMNAME_OPERTYPE = "opertype";
static const string XML_ITEMNAME_DEVNO = "DevNo";
static const string XML_ITEMNAME_TIMESTAMP = "TimeStamp";
static const string XML_ITEMNAME_CONTENT = "Content";
static const string XML_ITEMNAME_BLOCKLABEL = "BlockLabel";
static const string XML_ITEMNAME_MARKET = "Market";
static const string XML_ITEMNAME_PRICE = "Price";
static const string XML_ITEMNAME_SEQ = "Seq";
static const int CONNECTION_RETRY_TIMER = 3;



Consumer::Consumer()
{

}

Consumer::Consumer(const string& brname, int consumerNum) :
    m_brname(brname), m_market(Config::Instance()->m_deviceGroupId),
    m_consumerNum(consumerNum), pGetdataThr(NULL), m_seqRcv(0)
{
}
Consumer::~Consumer()
{
	stop();
}

string Consumer::getBrName()
{
	return m_brname;
}

int Consumer::getConumerNum()
{
	return m_consumerNum;
}

bool Consumer::run()
{
	m_shouldResume = true;

	if(NULL == pGetdataThr)
	{
		pGetdataThr = new std::thread(&Consumer::doGetData, this, m_consumerNum);
	}

	if(NULL == pGetdataThr)
	{
		_LOG_ERROR("new consumer thread fail[brname" << m_brname << "][market:"
			<< m_market << "][consumerNum" << m_consumerNum << "]");

		return false;
	}

	return true;
}

void Consumer::stop()
{
	m_shouldResume = false;

	if(pGetdataThr != NULL)
	{
		pGetdataThr->join();
		delete pGetdataThr;
		pGetdataThr = NULL;
	}
}

void Consumer::doGetData(int thrnum)
{
	_LOG_INFO("start get data,thread number:" << thrnum);

	std::unique_ptr<Face> p_face = nullptr;

	while(m_shouldResume)
	{
		string strName = "/yl/broadcast/baar/producer/" + m_brname  + "/"
		               + m_market + "/" + to_string(thrnum)
		               + "/" + to_string(m_seqRcv);

	    Interest interest(Name(strName.c_str()));
	    interest.setInterestLifetime(time::milliseconds(5000));
	    interest.setMustBeFresh(true);

		try
		{
			if(nullptr == p_face)
			{
				p_face = std::unique_ptr<Face>(new Face);
				if(nullptr == p_face)
				{
					continue;
				}
			}

			_LOG_DEBUG("express interest:" << strName);

			p_face->expressInterest(interest,
				bind(&Consumer::onData, this,  _1, _2),
				bind(&Consumer::onNack, this, _1, _2),
				bind(&Consumer::onTimeout, this, _1));

			p_face->processEvents();
		}
		catch (std::runtime_error& e)
		{
			_LOG_ERROR("nfd error:" << e.what());
			p_face = nullptr;
			sleep(CONNECTION_RETRY_TIMER);
		}
		catch(...)
		{
			_LOG_ERROR("conumer process events catch unknow error.");
			p_face = nullptr;
			sleep(CONNECTION_RETRY_TIMER);
		}
	}

	_LOG_INFO("stop getdata[brname:" << m_brname << "]");

	if(p_face != nullptr)
	{
		p_face->shutdown();
	}
}

void Consumer::onData(const Interest& interest, const Data& data)
{
	Block bck = data.getContent();
	size_t size = bck.value_size();
	//_LOG_INFO("bck:"<< (unsigned char *)bck.value());
	
 
    unsigned char   key[KEY_BYTE_NUMBERS];
    //Init vector.
    unsigned char   iv[KEY_BYTE_NUMBERS];

    /* A 128 bit IV */
    memcpy(key, KEY, KEY_BYTE_NUMBERS);
    memcpy(iv, INITIAL_VECTOR, KEY_BYTE_NUMBERS);

    int decryptedtext_len = -1,ciphertext_len = -1;
    ciphertext_len = size;

    unsigned char *ciphertextbuf = (unsigned char *)bck.value();

    // if (Config::Instance()->m_sCipherSwitch == 1)
    // {
    // hexdump(stdout,
    //     "=======ciphertextbuf=======",
    //     ciphertextbuf,
    //     ciphertext_len);
    // }

    unsigned char *decryptplaintextbuf=NULL;
    if (Config::Instance()->m_sCipherSwitch == 1)
    {
        decryptplaintextbuf=(unsigned char*)malloc(MAX_TEXT_SIZE*sizeof(char));
        try
        {
            decryptedtext_len = decrypt(ciphertextbuf, ciphertext_len, key, iv, decryptplaintextbuf);
        }
        catch (std::runtime_error& e)
        {
            _LOG_ERROR("producer decrypt data error:" << e.what());
        }
        catch(...)
        {
            _LOG_ERROR("producer decrypt data catch unknow error.");
        }
    }

    // if (Config::Instance()->m_sCipherSwitch == 1)
    // {
    // hexdump(stdout,
    //     "=======decryptplaintextbuf=======",
    //     decryptplaintextbuf,
    //     decryptedtext_len);
    // }
    //end decrypt the content by mali 20170606 add

    //begin decrypt the content by mali 20170606 modify
    char * pxml;
    if (Config::Instance()->m_sCipherSwitch == 1)
    {
      pxml = new char[decryptedtext_len + 1];
    }
    else
    {
      pxml = new char[bck.value_size() + 1];
    }

    if(NULL == pxml)
    {
        _LOG_ERROR("cann't new char that pxml point to");

        if (Config::Instance()->m_sCipherSwitch == 1)
        {
            free(decryptplaintextbuf);
        }
        return;
    }

    if (Config::Instance()->m_sCipherSwitch == 1)
    {
        memset(pxml, '\0', decryptedtext_len + 1);
        memcpy(pxml, reinterpret_cast<const char*>(decryptplaintextbuf), decryptedtext_len);
        free(decryptplaintextbuf);
    }
    else
    {
        memset(pxml, '\0', bck.value_size() + 1);
        memcpy(pxml, reinterpret_cast<const char*>(bck.value()), bck.value_size());
    }
	_LOG_INFO("pxml:"<<pxml);


    // //end decrypt the content by mali 20170606 modify

	TiXmlDocument doc;
	doc.Parse(pxml);
	printf("Jerry test: bsdr consumer %s\n", pxml);

	_LOG_INFO("Jerry test: bsdr consumer:" << pxml);
	delete[] pxml;
//	return;
      
	if(doc.Error())
	{
		_LOG_ERROR("parse xml doc error:" << doc.ErrorDesc());
		return;
	}

	TiXmlElement* root = doc.RootElement();

	TiXmlNode* itemSeq = root->FirstChild("Seq");
	if(itemSeq != NULL)
	{
		const char* pText = itemSeq->ToElement()->GetText();
		m_seqRcv = atoi(pText);
	}
//	return ;

	for(TiXmlNode* item = root->FirstChild("Item"); item; item = item->NextSibling("Item"))
	{
		TiXmlNode* child = item->FirstChild();
		if(NULL == child)
		{
			continue;
		}

		ChainBlockData *pData = new ChainBlockData();
		ConsumerData cdata;
	
		//设置新字段
		
		if(NULL == pData)
		{
			_LOG_ERROR("Consumer::onData error[cannt new BlockData]");
			continue;
		}


		while(child != NULL)
		{
			const char* pEleText = child->ToElement()->GetText();
			const string eleName = child->Value();

			string strEleText("");
			if(pEleText != NULL)
			{
				strEleText = pEleText;
			}

			// _LOG_DEBUG("[ItemName:" << eleName << "][ItemValue:" << strEleText << "]");
			if("param"==eleName){
				cdata.JasonParam = std::string(strEleText);
			}

			if("OperType"==eleName){
				cdata.OperType = atoi(strEleText.c_str());
			}


			child = item->IterateChildren(child);
		}

		//通过方法表顺序进行 block构造if(PRODUCER_DATA_FUNC::)
	
		writeJsonToChainData(cdata,pData);
		BPM::Instance()->put(pData);

	}

}

void Consumer::onNack(const Interest& interest, const lp::Nack& nack)
{
	_LOG_ERROR("get date on nack[" << interest << "]");
	// p_face = nullptr;
	sleep(CONNECTION_RETRY_TIMER);
}

void Consumer::onTimeout(const Interest& interest)
{
	_LOG_ERROR("get date timeout[" << interest << "]");
}

std::ostream& operator<<(std::ostream& os, const Consumer& consumer)
{
	os << "[brname:" << consumer.m_brname << "][market:"
	   << consumer.m_market << "][conmerNum:" << consumer.m_consumerNum
	   << "][seq:" << consumer.m_seqRcv
	   << "]" ;

	return os;
}

int Consumer::writeJsonToChainData(const ConsumerData &input,ChainBlockData *output){

		_LOG_INFO("begin writeJsonToChainData");
		_LOG_INFO(input.JasonParam);

	
		Json::Reader reader;
		Json::Value value;
		output->func = input.OperType;
	
		if (reader.parse(input.JasonParam.c_str(), value))
		{
			output->tid = value["TID"].asString();
			output->virtualId = value["VirtualId"].asString();
			output->authToken = value["AuthToken"].asString();
			output->blocklabel = value["BlockLabel"].asString();
			output->market = value["ProviderMarket"].asString();
			output->deviceGroupId = value["DeviceGroupId"].asString();
			output->servfee = value["ServFee"].asString();
			output->price = value["Price"].asString();
			output->s3srcFileName = value["S3SrcFileName"].asString();
			output->s3srcFileSize = value["S3SrcFileSize"].asUInt64();
			output->verification = value["Verification"].asString();
			output->content = value["Content"].toStyledString();
	
			
			return 0;
		}
		return -1;
}

// int Consumer::writeJsonToMarketData(const ConsumerData &input,MarketBlockData *output){

// 		_LOG_INFO("begin writeJsonToMarketData");

// 		Json::Reader reader;
// 		Json::Value value;
// 		output->func = input.OperType;
// 		//解析方法
// 		if (reader.parse(input.JasonParam, value))
// 		{
			
// 				output->deviceGroupId = value["DeviceGroupId"].asString();
// 				output->balance = value["InitBalance"].asString();
// 				output->market = value["Market"].asString();
// 				output->servfee = value["ServFee"].asString();
// 				return 0;
// 		}
		
// 	return -1;
// }
