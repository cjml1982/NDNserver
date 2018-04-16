#ifndef YL_BAAR_CONFIG
#define YL_BAAR_CONFIG

#include <string>
#include <vector>
#include "tinyxml/tinyxml.h"
#include "./constant/myconstant.hpp"

static const string RGWFILE = "rgw.xml";
//michael 20180311 added
static const string JWTFILE = "jwt.xml";

using namespace std;

class Config
{
protected:
	Config();
	~Config();

private:
	static Config* pConfig;
public:
	static Config* Instance();

public:
	bool ReadConfig(const string& sCfgFileName = "");


	//begin add by liangyuanting 20171204
	bool ReadPortConfig(const string& sCfgFileName = "");
	//end add by liangyuanting 20171204

	static bool readRGWConfig();

	//michael 20180311 added
	bool readJWTConfig();

public:
	string m_deviceGroupId;
	int m_sBnrTimer;       // 
	int m_cosumerThrNum;   // consumer thread number
	int m_sProThrNum;      // producer thread number
	int m_bpmThrNum;       // business process thread number;

	int m_mysqlPoolSize;   // mysqlpool size
	int m_asynThrSize;
	ndn::FXUtil futil;

  //begin cipher switch by liulijin 20170606 add
  int m_sCipherSwitch;
  //end cipher switch by liulijin 20170606 add

  typedef struct RGW
  {
	 string user, host, accessKey, secretKey;
	 vector<string> buckets;
  }RGW;

  static vector<RGW *> rgwVec;


  //begin add by liangyuanting 20171204
   vector<TiXmlElement*> portVec;
  //end add by liangyuanting 20171204

  std::string myip;

  vector<ndn::DBUtil> dbvec;

  //michael 20180311 added
  struct JWTConfig
  {
    std::string ip;
    std::string port;
  };
  vector<JWTConfig> jwtcfg;

};




#endif	/* #ifndef YL_BAAR_CONFIG */
