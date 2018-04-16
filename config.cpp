#include "config.h"
#include "tinyxml/tinyxml.h"
#include "logging4.h"

INIT_LOGGER("Config");

std::vector<Config::RGW *> Config::rgwVec(0);

Config* Config::pConfig = new Config();

Config* Config::Instance()
{
	return pConfig;
}

Config::Config()
{

}
Config::~Config()
{

}


bool Config::ReadConfig(const string& sCfgFileName)
{
	TiXmlDocument doc(sCfgFileName.c_str());
	bool loadOkay = doc.LoadFile();
	
	TiXmlHandle hDoc(&doc);
	TiXmlElement* pElem;
	TiXmlHandle hRoot(0);

	// block: name
	{
		pElem=hDoc.FirstChildElement().Element();
		// should always have a valid root but handle gracefully if it does
		if (!pElem) return false;
		
		// save this for later
		hRoot=TiXmlHandle(pElem);
	}


	if(!loadOkay){
		_LOG_ERROR("Could not load config file[" << sCfgFileName << "].Error='"
			<< doc.ErrorDesc() << "'.Exiting.");
		return false;
	}

	TiXmlElement* root = doc.RootElement();

	TiXmlElement* eleDevicegroupid = hRoot.FirstChild("DEVICEGROUPID").Element();
	if(NULL == eleDevicegroupid)
	{
		BN_LOG_ERROR("Element[DEVICEGROUPID] not exist.");
		return false;
	}
	if(NULL == eleDevicegroupid->Attribute("ID"))
	{
		BN_LOG_ERROR("Attribute[Element:DEVICEGROUPID][Attr:ID] not exist.");

		return false;
	}
	else
	{

		m_deviceGroupId = eleDevicegroupid->Attribute("ID");

	}

	TiXmlElement* eleNamePub = hRoot.FirstChild("NAMEPUB").Element();
	if(NULL == eleNamePub)
	{
		BN_LOG_ERROR("Element[NAMEPUB] not exist.");
		return false;
	}
	if(NULL == eleNamePub->Attribute("ReExpressTimer", &m_sBnrTimer))
	{
		BN_LOG_ERROR("Attribute[Element:NAMEPUB][Attr:ReExpressTimer] not exist.");
		return false;
	}

	TiXmlElement* eleBpm = hRoot.FirstChild("BPM").Element();
	if(NULL == eleBpm)
	{
		BN_LOG_ERROR("Element[BPM] not exist.");
		return false;
	}
	if(!eleBpm->Attribute("ThreadNumber", &m_bpmThrNum))
	{
		BN_LOG_ERROR("Attribute[Element:BPM][Attr:ThreadNumber] not exist.");
		return false;
	}

	TiXmlElement* eleConsumer = hRoot.FirstChild("CONSUMER").Element();
	if(NULL == eleConsumer)
	{
		BN_LOG_ERROR("Element[CONSUMER] not exist.");
		return false;
	}
	if(!eleConsumer->Attribute("ThreadNumber", &m_cosumerThrNum))
	{
		BN_LOG_ERROR("Attribute[Element:CONSUMER][Attr:ThreadNumber] not exist.");
		return false;
	}

	TiXmlElement* eleProducer = hRoot.FirstChildElement("PRODUCER").Element();
	if(NULL == eleProducer)
	{
		BN_LOG_ERROR("Element[PRODUCER] not exist.");
		return false;
	}
	if(!eleProducer->Attribute("ThreadNumber", &m_sProThrNum))
	{
		BN_LOG_ERROR("Attribute[Element:PRODUCER][Attr:ThreadNumber] not exist.");
		return false;
	}

	TiXmlElement* eleMysqlPool = hRoot.FirstChild("MYSQLPOOL").Element();
	if(NULL == eleMysqlPool)
	{
		BN_LOG_ERROR("Element[MYSQLPOOL] not exist.");
		return false;
	}
	if(!eleMysqlPool->Attribute("MySQLPoolSize", &m_mysqlPoolSize))
	{
		BN_LOG_ERROR("Attribute[Element:MYSQLPOOL][Attr:MySQLPoolSize] not exist.");
		return false;
	}


	TiXmlElement* eleAsynthrNum = hRoot.FirstChild("ASYNTHREAD").Element();
	if(NULL == eleAsynthrNum)
	{
		BN_LOG_ERROR("Element[ASYNTHREAD] not exist.");
		return false;
	}
	if(NULL == eleAsynthrNum->Attribute("AsynThreadSize", &m_asynThrSize))
	{
		BN_LOG_ERROR("Attribute[Element:ASYNTHREAD][Attr:AsynThreadSize] not exist.");
		return false;
	}


  //begin cipher switch by liulijin 20170606 add
  TiXmlElement* eleCipher =  hRoot.FirstChild("CIPHERS").Element();
  if(NULL == eleCipher)
  {
    BN_LOG_ERROR("Element[CIPHERS] not exist.");
    return false;
  }
  if(NULL == eleCipher->Attribute("SwitcherNo", &m_sCipherSwitch))
  {
    BN_LOG_ERROR("Attribute[Element:CIPHERS][Attr:SwitcherNo] not exist.");
    return false;
  }

  //法信服务器
  TiXmlElement *fxServer =  hRoot.FirstChild("FXSERVER").Element();
  if(NULL == fxServer)
  {
	BN_LOG_ERROR("Element[FXSERVER] not exist.");
   	return false;
  }

  futil.fip  = fxServer->Attribute("fip");
  futil.fport  = fxServer->Attribute("fport");
  
  //自身ip的获取
  TiXmlElement *myAddress =  hRoot.FirstChild("MYADDRESS").Element();
  if(NULL == fxServer)
  {
	BN_LOG_ERROR("Element[MYADDRESS] not exist.");
   	return false;
  }
  myip = myAddress->Attribute("myip");


  TiXmlElement *dbServer =  hRoot.FirstChild("DBSERVER").Element();
  if(NULL == dbServer)
  {
	BN_LOG_ERROR("Element[DBSERVER] not exist.");
   	return false;
  }
  for(;NULL!=dbServer;dbServer=dbServer->NextSiblingElement())
  {

	  if(NULL==dbServer->Attribute("dbip"))
	  {
		  continue;
	  }
	  ndn::DBUtil db;
   	  db.ip = dbServer->Attribute("dbip");
	  dbvec.push_back(db);

  }
    BN_LOG_INTERFACE("finish read bsdrca.xml");
	return true;
}

//begin add by liangyuanting 20171204
bool Config::ReadPortConfig(const string& sCfgFileName){

	BN_LOG_INTERFACE("begin reading port config;");
	TiXmlDocument doc(sCfgFileName.c_str());
	bool loadOkay = doc.LoadFile();

	if(!loadOkay){
		_LOG_ERROR("Could not load config file[" << sCfgFileName << "].Error='"
			<< doc.ErrorDesc() << "'.Exiting.");
		return false;
	}

	TiXmlElement* root = doc.RootElement();

	for(TiXmlElement* elem = root->FirstChildElement("node");NULL!=elem;elem=elem->NextSiblingElement())
	{
		  if(NULL == elem->Attribute("port"))
     	 {
          //fprintf(stderr, "loadXMLtoCreateWallet port empty!\n");
         // _LOG_INFO("loadXMLtoCreateWallet port empty!");
          continue;
     	 }
		TiXmlElement * tmpElem ;
        tmpElem =(TiXmlElement *) elem->Clone();
		portVec.push_back(tmpElem);

	}
	_LOG_INFO("portSize:"<<portVec.size());	
	BN_LOG_INTERFACE("finish load the portxml");	

	return true;

}
//end add by liangyuanting 20171204

bool Config::readRGWConfig()
{
	BN_LOG_INTERFACE("begin reading rgw config;");
	const string file = RGWFILE;
	TiXmlDocument doc(file.c_str());
	bool loadOkay = doc.LoadFile();

	if(!loadOkay){
		_LOG_ERROR("Could not load config file[" << file << "].Error='"
			<< doc.ErrorDesc() << "'.Exiting.");
		return false;
	}

	TiXmlElement* root = doc.RootElement();
	if(NULL == root)
    {
		BN_LOG_ERROR("Get root element failed.");
		return false;
    }
	string hostname;
	TiXmlElement* host = root->FirstChildElement("host");
	if(NULL == host)
	{
		BN_LOG_ERROR("Get host element failed.");
		return false;
	}
	hostname = host->GetText();
	TiXmlNode* users = root->FirstChild("users");
	if(NULL == users)
	{
		BN_LOG_ERROR("Node[users] not exist.");
		return false;
	}
	TiXmlNode* usern = NULL;
	for(usern = users->FirstChild(); usern != 0; usern = usern->NextSibling()){
		RGW * rgw = new RGW;
		rgw->host = hostname;
		if(NULL == usern)
		{
			BN_LOG_ERROR("Get user node failed.");
			return false;
		}
		TiXmlElement* user = usern->ToElement();
		if(NULL == user)
		{
			BN_LOG_ERROR("Get user element failed.");
			return false;
		}
		rgw->user = user->Attribute("name");
		TiXmlElement* access = user->FirstChildElement("access_key");
		if(NULL == access)
		{
			BN_LOG_ERROR("Get access element failed.");
			return false;
		}
		rgw->accessKey = access->GetText();
		TiXmlElement* secret = user->FirstChildElement("secret_key");
		if(NULL == secret)
		{
			BN_LOG_ERROR("Get secret element failed.");
			return false;
		}
		rgw->secretKey = secret->GetText();
		TiXmlNode* buckets = user->FirstChild("buckets");
		if(NULL == buckets)
		{
			BN_LOG_ERROR("Get buckets node failed.");
			return false;
		}
		TiXmlNode* buckete = NULL;
		for(buckete = buckets->FirstChild(); buckete != 0; buckete = buckete->NextSibling())
		{
			if(NULL == buckete)
			{
				BN_LOG_ERROR("Get bucket node failed.");
				return false;
			}
			TiXmlElement* bucket = buckete->ToElement();
			if(NULL == bucket)
			{
				BN_LOG_ERROR("Get bucket element failed.");
				return false;
			}
			rgw->buckets.push_back(bucket->GetText());
		}
		rgwVec.push_back(rgw);
	}

	BN_LOG_INTERFACE("finish load the rgw_xml");

	return true;
}

//michael 20180311 added
bool Config::readJWTConfig()
{
  BN_LOG_INTERFACE("begin reading JWT config;");
  const string file = JWTFILE;
  TiXmlDocument doc(file.c_str());
  bool loadOkay = doc.LoadFile();

  if(!loadOkay){
    _LOG_ERROR("Could not load config file[" << file << "].Error='"
      << doc.ErrorDesc() << "'.Exiting.");
    return false;
  }

  TiXmlElement* root = doc.RootElement();
  if(NULL == root)
  {
    BN_LOG_ERROR("Get root element failed.");
    return false;
  }

  std::string ip;
  std::string port;

  TiXmlElement* ipnode = root->FirstChildElement("ip");
  if(NULL == ipnode)
  {
    BN_LOG_ERROR("Get ip element failed.");
    return false;
  }
  ip = ipnode->GetText();

  TiXmlElement* portnode = root->FirstChildElement("port");
  if(NULL == portnode)
  {
    BN_LOG_ERROR("Get port element failed.");
    return false;
  }
  port = portnode->GetText();

  JWTConfig jc;
  jc.ip = ip;
  jc.port = port;
  jwtcfg.push_back(jc);

  BN_LOG_INTERFACE("finish load the jwt_xml");

  return true;
}
