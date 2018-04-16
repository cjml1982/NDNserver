#include <stdio.h>
#include <string.h>
#include <iostream>

#include "producermgr.h"
#include "consumermgr.h"
#include "bpm.h"
#include "bsdrnamepub.h"

//begin create table by liulijin 20170602
#include "mybiz.hpp"

#include "mybizpl.hpp"
//end create table by liulijin 20170602
#include "mywallet.h"
#include "mysql/myinstqueue.hpp"

//deleted by michael, 20180306


using namespace std;

INIT_LOGGER("bsdrsvr");

volatile bool shouldrun = true;


class DbItem{
  public:
  std::string Type;
  int 
  (ndn::CMyBIZ::*process)();
};

static DbItem dbItems[]={

  {"--ccdb",&ndn::CMyBIZ::createMySQLDB},
  {"--ccdata",&ndn::CMyBIZ::createMySQLDataTable},
  {"--cctx",&ndn::CMyBIZ::createMySQLTXTable},
  {"--ccmkt",&ndn::CMyBIZ::createMarketTable},
  {"--ccfl",&ndn::CMyBIZ::createFileTable}


};

int doMethod(){
  int len = sizeof(dbItems)/sizeof(dbItems[0]);
  for(int i = 0;i<len;i++)
  {
      (ndn::CMyBIZ::getInstance()->*(dbItems[i].process))();    
  }
  return 0;
};

int main(int argc, char *argv[])
{

  int bizRet = 0;

   InitBnLog("BSDR", BN_LOG_MODULE_BSDR);

   BN_LOG_INTERFACE("bsdrsvr start...");
  if(!Config::readRGWConfig())
  {
	  //BN_LOG_ERROR("read config file[rgw.xml] error.");
    BN_LOG_ERROR("read config file[rgw.xml] error.");
	  return -1;
  }

  //michael 20180311 added
  if(!Config::Instance()->readJWTConfig())
  {
    BN_LOG_ERROR("read config file[jwt.xml] error.");
    return -1;
  }

  //begin modified the code by liangyuanting 20170817
	if(!Config::Instance()->ReadConfig(__XMLFILE))
	{
		BN_LOG_ERROR("read config file[bsdrca.xml] error.");
    
		return -1;
	}

  if(!Config::Instance()->ReadPortConfig(_XMLFILE_PORT))
	{
		BN_LOG_ERROR("read config file[bsdrport.xml] error.");
		return -1;
	}


 //end modified the code by liangyuanting 20170817

  //连接池初始化1.2.
  bizRet = ndn::CMyBIZ::getInstance()->initMainTLib();
  if (bizRet == -1)
  {
    BN_LOG_ERROR("sql lib init failure!");
    return -1;
  }

  bizRet = ndn::CMyWallet::getInstance()->initWalletHashMap(__XMLFILE);
  if (bizRet == -1)
  {
    BN_LOG_ERROR("init wallet map failure!");
    return -1;
  }
  

  try{
       doMethod();
       ndn::CMyWallet::getInstance()->initMySQLPool();
       //开始加载,未成功上链的记录
       ndn::CMyWallet::getInstance()->reChain();

  }catch(CMy_BSDR_Exception *e)
  {

    BN_LOG_ERROR("THE DB CONNECTION IS ERROR !PLEASE CHECK THE DB SERVER!AND REOPEN THIS APPLICATIION!");
    delete e;
    e = NULL;
    return -1;
  }

 
  bizRet = ndn::CMyCURLPL::getInstance()->curlInit();
  if (bizRet == -1)
  {
    BN_LOG_ERROR("curl global init failure!");
    return -1;
  }
    //bizRet = ndn::CMyBIZ::getInstance()->initWalletTable(__XMLFILE);

 


  bizRet = ndn::CMyWallet::getInstance()->runReader();
  if (bizRet == -1)
  {
    BN_LOG_ERROR("create thread error!");
    return -1;
  }
  //end create table by liulijin 20170602 added

	
  BsdrNamePub namepub;
  if(!namepub.run())
  {
    BN_LOG_ERROR("run name publisher fail.");
    return -1;
  }

	ProducerMgr proMgr;
  if(!proMgr.run())
  {
    BN_LOG_ERROR("run producer manager fail.");
    return -1;
  }

  usleep(100);

	ConsumerMgr conMgr;
  if(!conMgr.run())
  {
    BN_LOG_ERROR("run consumer manager fail.");
    return -1;
  }

	if(!BPM::Instance()->run())
  {
    BN_LOG_ERROR("run BPM fail.");
    return -1;
  }
	while(shouldrun)
	{
		sleep(1);
	}

	//begin modified by michael 20170814
	//连接池销毁队列销毁4.5.
	ndn::CMyBIZPL::getInstance()->cqiHolderDestroy();
	//end modified by michael 20170814
 // ndn::CMemcached::getInstance()->destroyMemcachedPool();


    return (0);
}
