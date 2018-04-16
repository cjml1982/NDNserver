
#include "mybizpl.hpp"
#include "userauth/myjwt.hpp"
#include "s3file.h"


INIT_LOGGER("CMyBIZPL");



static string TYPE_GET_REG_DATA = "getregdata"; 
static string TYPE_GET_REG_DATA_SIZE = "getregdatasize";
static string TYPE_GET_REG_DATA_PRICE = "getregdataprice";
static string TYPE_UPDATE_REG_DATA_PRICE = "updateregdataprice";


static string TYPE_GET_MARKET_FROM_MARKET ="getmarketinfofrommarket";
static string TYPE_GET_MARKET_FROM_USER    = "getmarketinfofromuser";
static string TYPE_GET_MARKET_FROM_TEC_COMPANY = "getmarketinfofromteccomapany";
static string TYPE_GET_MARKET_FROM_TEC_COMPANY_AS_USER = "getmarketinfofromteccomapanyasuser";
static string TYPE_GET_REG_DATA_FROM_OTHER_USER  = "getregdatafromotheruser";

static string TYPE_GET_CONSUME_BILL    = "getconsumebill";
static string TYPE_GET_CONSUME_SUM    = "getconsumesum";



static string TYPE_REQ_USER_PUBKEY_SYNC    = "requestuserpubkeysync";
static string TYPE_GET_AUTH_TOKEN    = "getauthtoken";
static string TYPE_GET_AUTH_RANDOM   = "getauthrandom"; //Jerry add 20180319


static string TYPE_QUERY_MARKET_BN   = "getmarketinfofrombn" ;// Jerry add 20180319

static string TYPE_UPDATE_MARKET_BALANCE_FROM_BN_NODE = "updatemarketbalancefrombnnode";// Jerry add 20180404
static string TYPE_ADD_MARKET_TO_BN_NODE ="addmarkettobnnode";
static string TYPE_DISABLE_MARKET_FROM_BN_NODE ="disablemarketfrombnnode";






static  ndn::CRegMethodItem RegMethodItems[]={
            {TYPE_GET_REG_DATA,&ndn::CMyBIZPL::queryRegExist},//4.4 已登记数据查询核对 
            {TYPE_GET_REG_DATA_SIZE,&ndn::CMyBIZPL::queryRegSum},//4.5 已登记数据量查询 
            {TYPE_GET_REG_DATA_PRICE,&ndn::CMyBIZPL::queryRegPrice},//4.7 已登记数据定价查询 
            {TYPE_UPDATE_REG_DATA_PRICE,&ndn::CMyBIZPL::mainUpdateRegPrice},//4.7 已登记数据定价查询 
            
};


static ndn::CPayMethodItem PayMethodItems[]={
       {TYPE_GET_MARKET_FROM_MARKET,&ndn::CMyBIZPL::queryMarketFromMarket},//5.1 市场A查询市场B 
       {TYPE_GET_MARKET_FROM_USER,&ndn::CMyBIZPL::queryMarketFromUser},//5.2 用户1查询市场A
       {TYPE_GET_REG_DATA_FROM_OTHER_USER,&ndn::CMyBIZPL::queryMarketFromOtherUser},//5.3,5.4技术确认
       {TYPE_GET_MARKET_FROM_TEC_COMPANY,&ndn::CMyBIZPL::queryMarketFromTecCompany},//5.5 技术公司查询市场A
       {TYPE_GET_MARKET_FROM_TEC_COMPANY_AS_USER,&ndn::CMyBIZPL::queryMarketFromTecCompanyAsUser},//5.6 技术公司作为用户查询市场A
       {TYPE_GET_CONSUME_BILL,&ndn::CMyBIZPL::queryConsumeBill},//查询账单
       {TYPE_GET_CONSUME_SUM,&ndn::CMyBIZPL::queryConsumeSum}//查询账单量
};



static ndn::CAuthTokenItem AuthTokenItems[] = {
       {TYPE_GET_AUTH_TOKEN,&ndn::CMyBIZPL::getAuthTokenfromSig},
       {TYPE_REQ_USER_PUBKEY_SYNC,&ndn::CMyBIZPL::reqPubkeySync},
       {TYPE_GET_AUTH_RANDOM,&ndn::CMyBIZPL::getAuthRandom}
};

static ndn::CMarketMethodItem MarketUnitMethodItems[] = {
  
      {TYPE_QUERY_MARKET_BN,&ndn::CMyBIZPL::queryMarketFromBn},
      {TYPE_UPDATE_MARKET_BALANCE_FROM_BN_NODE,&ndn::CMyBIZPL::changeMarketBalance},
      {TYPE_DISABLE_MARKET_FROM_BN_NODE,&ndn::CMyBIZPL::deleteMarketInfo},
      {TYPE_ADD_MARKET_TO_BN_NODE,&ndn::CMyBIZPL::insertMarketInfo}

};


static std::string mstr[]={

    "getregdata",
    "getregdatasize",
    "getregdataprice",
    "updateregdataprice",

    "getregdatafromotheruser",
    "getmarketinfofrommarket",
    "getmarketinfofromuser",
    "getmarketinfofromteccomapany",
    "getmarketinfofromteccomapanyasuser",
    "getconsumebill",
    "getconsumesum",
    



    "requestuserpubkeysync",
    "getauthtoken",
    "getauthrandom",

    "getmarketinfofrombn",
    "disablemarketfrombnnode",
    "addmarkettobnnode",
    "updatemarketbalancefrombnnode"



};

int
ndn::CMyBIZPL::methodProcess(ProducerData &data,std::string &content)
{  

    std::string methodType = data.strOperType;
    std::string jparm = data.param;
    //对转义进行处理
    

    
    int errCode = 0;
    int len = sizeof(mstr)/sizeof(mstr[0]);
    for(int i = 0;i<len;i++)
    {
        if(mstr[i] == methodType)
        {
            //对登记查询操作
            if(0<=i&&i<4)
            {
                for(int j = 0;j<sizeof(RegMethodItems)/sizeof(RegMethodItems[0]);j++)
                {
                    if(RegMethodItems[j].type == methodType)
                    {
                        //包装为Creg

                        CReg creg;
                        
                        // data.param = data.param.erase(0,1);
                        // data.param = data.param.erase(data.param.size()-1,1);
                        // _LOG_INFO("onInterest1:" << data.param);
                        writeJsonToCReg(data,creg);
                        errCode = (ndn::CMyBIZPL::getInstance()->*(RegMethodItems[j].processMethod))(creg,content);
                        if(-1 == errCode)
                            return -1;
                    }
                    
                }
            }
            //market操作
            if(4<=i&&i<11)
            {
                for(int j = 0;j<sizeof(PayMethodItems)/sizeof(PayMethodItems[0]);j++)
                {
                    if(PayMethodItems[j].type == methodType)
                    {
                        CPay cpay;
                        // data.param = data.param.erase(0,1);
                        // data.param = data.param.erase(data.param.size()-1,1);
                        // _LOG_INFO("onInterest1:" << data.param);

                        writeJsonToCPay(data,cpay);
                        errCode = (ndn::CMyBIZPL::getInstance()->*(PayMethodItems[j].processMethod))(cpay,content);
                        if(-1 == errCode)
                            return -1;
                    }
                    
                }

             
            }
            //authToken 操作
            if(11<=i&&i<14)
            {
                   for(int j = 0;j<sizeof(AuthTokenItems)/sizeof(AuthTokenItems[0]);j++)
                    {
                        if(AuthTokenItems[j].type == methodType)
                        {
                            CAuthUser  cAuthUser;
                            writeJsonToCAuthToken(data,cAuthUser);
                            errCode = (ndn::CMyBIZPL::getInstance()->*(AuthTokenItems[j].processMethod))(cAuthUser,content);
                            if(-1 == errCode)
                                return -1;
                        }
                        
                    }
            }
            if(14<=i)
            {
                   for(int j = 0;j<sizeof(MarketUnitMethodItems)/sizeof(MarketUnitMethodItems[0]);j++)
                    {
                        if(MarketUnitMethodItems[j].type == methodType)
                        {
                            CMarket  cmarket;
                            writeJsonToCMarket(data,cmarket);
                            errCode = (ndn::CMyBIZPL::getInstance()->*(MarketUnitMethodItems[j].processMethod))(cmarket,content);
                            if(-1 == errCode)
                                return -1;
                        }
                        
                    }
            }
            
            return 0;
        }
      
    }
    return -1;
}
//end by liangyuanting


ndn::CMyBIZPL * ndn::CMyBIZPL::instance = new CMyBIZPL;

ndn::CMyBIZPL *
ndn::CMyBIZPL::getInstance()
{
  return instance;
}


int
ndn::CMyBIZPL::cqiHolderDestroy()
{
  BN_LOG_INTERFACE("begin cqiHolderDestroy");

  int qsize = 0;
  ndn::CMyInstQueue::getInstance()->qGetSize(qsize);

  for (int i = 0; i <qsize;i++)
  {
    CQIHolder * itm = (CQIHolder *)ndn::CMyInstQueue::getInstance()->qPop();
    
    if (NULL != itm)
    {
      //if (NULL != list)
      //  curl_slist_free_all(list);
      curl_easy_cleanup(itm->curl);

      mysql_close(itm->client);
      //mysql_thread_end();

      delete itm;
      itm = NULL;
    }
  }
  curl_global_cleanup();
 // mysql_library_end();

  BN_LOG_INTERFACE("end cqiHolderDestroy");
  return 0;
}


//added by liangyuanting 




int ndn::CMyBIZPL::mainPay(CQIHolder *holder,CPay &cpay,S3FileInfo &s3fi){

    initMysqlHolder(holder);
    int blcount = 0;
    int isAvailable = 0;
    int resFail = 0;
    //groupId 与 providerMarket检查
    checkGroupIdAndMarket(cpay);
   
    //consumerMarket是否存在
    if(0==marketCount(cpay.consumerMarket)){
    return -1;
    };

    //是否存在blockLabel，是否被禁用
    CDealUnit tmpUnit = (CDealUnit)cpay;
    checkBlockLabelAndAvailableType(holder,tmpUnit,blcount,isAvailable);
    cpay.price = tmpUnit.price;
    cpay.verification = tmpUnit.verification;
    if(1 == blcount&&1 == isAvailable){
        
      //看金钱时候满足
      int cprice = marketBalance(cpay.providerMarket);
      if(cprice<atoi((cpay.price).c_str()))
      {
        return -1;
      }

      std::string bcrURL("");
      std::string bcrHeader("");
      std::string bcrAddress("");
      std::string bcrChainId("");
      std::string txid("");

      if(-1 == cpay.ackey){
        cpay.ackey = getACKey(cpay.blocklabel);
      }

      //获取链的情况
      getFromWalletArray(cpay.ackey,bcrURL,bcrHeader,bcrAddress,bcrChainId);


      
      getSendToAddressForRightV2Demo(holder,bcrURL, bcrHeader, cpay.verification, bcrAddress,txid,__LOWEST_AMOUNT,bcrChainId);

      BN_LOG_INTERFACE("iTxId: %s", txid.c_str());
      //获得上链id后 进行对tx表进行更新
      resFail = ndn::CMyMYSQLPL::getInstance()->addRecordsTX(holder,
                                                            cpay,
                                                            txid);
    
      {
       
        CMarket * c1 = getMarketFromHashMap(cpay.providerMarket);
        CMarket * c2 = getMarketFromHashMap(cpay.consumerMarket);
        if((mylocalname != c1->deviceGroupId)||(0 == marketCount(c1->market)))
        {	
          throw new CMy_BSDR_Exception("__GLOBAL_NO_MARKET_ERR",__GLOBAL_NO_MARKET_ERR);
        }
        if((mylocalname != c2->deviceGroupId)||(0 == marketCount(c2->market)))
        {
          throw new CMy_BSDR_Exception("__GLOBAL_NO_MARKET_ERR",__GLOBAL_NO_MARKET_ERR);
        }

        std::lock_guard<std::mutex> lg(mtx_market);
        c1->balance -=atoi((cpay.price).c_str());
        c2->balance +=atoi((cpay.price).c_str());
        updateMoney(holder,c1,c2);

      }
      ndn::CMyMYSQLPL::getInstance()->selectS3File(holder,s3fi);

      return 0;
        
    }

    return -1;

  
}


//4.4 已登记数据查询核对
int
ndn::CMyBIZPL::queryRegExist(CReg &reg,std::string &outputJson)
{
    std::string res;
    //TODO
    try{
      
      reg.checkBanOrReviewOrQuery();
      HolderRaii holderRaii;
      CQIHolder *holder = holderRaii.getMysqlHolder();
      
      if(-1 == checkGroupIdAndMarket(reg))
      {
        return -1;
      };
      int blcount = 0;
      int isAvailable = 0;
      initMysqlHolder(holder);
      CAuthUser user;
      user.virtualId = reg.virtualId;
      user.authToken = reg.authToken;
      checkAuthToken(holder,user);
      CDealUnit tmpUnit = (CDealUnit)reg;
      checkBlockLabelAndAvailableType(holder,tmpUnit,blcount,isAvailable);

      if(0 == blcount||0 == isAvailable)
      {
        throw new CMy_BSDR_Exception("__GLOBAL_BLOCKLABEL_ERR",__GLOBAL_BLOCKLABEL_ERR);
      }
      if(1 == blcount&& 1 == isAvailable)
      {
        //存在
        //进行获取登记的值的条
        RegResult rreg;
        ndn::CMyMYSQLPL::getInstance()->queryRegRecords(holder,reg,rreg);
        std::string ResultCode;
        std::string BlockLabel;
        std::string ProviderMarket;
        std::string DeviceGroupId;
        std::string Price;
        std::string rs = std::string("");
        
        //取s3接口问题
        S3File s3file;
        //查询表
        S3FileInfo s3fi;
        s3fi.blocklabel = reg.blocklabel;
        ndn::CMyMYSQLPL::getInstance()->selectS3File(holder,s3fi);
        //不需要走getfile
        res.append("{");
        res.append(writeResultJson("TID",reg.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode","1"));
        res.append(",");
        res.append(writeResultJson("BlockLabel",reg.blocklabel));
        res.append(",");
        res.append(writeResultJson("ProviderMarket",reg.providerMarket));
        res.append(",");
        res.append(writeResultJson("DeviceGroupId",reg.deviceGroupId));
        res.append(",");
        res.append(writeResultJson("Price",rreg.price));
        res.append(",");
        res.append(writeResultJson("Content",rreg.content));
        res.append(",");
        res.append(writeResultJson("S3Action","4"));
        res.append(",");
        res.append(writeResultJson("S3User",s3fi.s3User));
        res.append(",");
        res.append(writeResultJson("S3Access",s3fi.s3Access));
        res.append(",");
        res.append(writeResultJson("S3Secret",s3fi.s3Secret));
        res.append(",");
        res.append(writeResultJson("S3Host",s3fi.s3Host));
        res.append(",");
        res.append(writeResultJson("S3Bucket",s3fi.s3Bucket));
        res.append(",");
        res.append(writeResultJson("S3DstFile",s3fi.s3DstFile)); 
        res.append("}");
        outputJson = res;
        return 0;
      }

    }catch(CMy_BSDR_Exception *e){

        res.append("{");
        res.append(writeResultJson("TID",reg.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
        res.append(",");
        res.append(writeResultJson("BlockLabel",reg.blocklabel));
        res.append(",");
        res.append(writeResultJson("ProviderMarket",reg.providerMarket));
        res.append(",");
        res.append(writeResultJson("DeviceGroupId",reg.deviceGroupId));
        res.append("}");
        outputJson =  res;
        BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
        delete e;
        e = NULL;
        return -1;
    }
      //s3接口接入
      return -1;
}

//
int ndn::CMyBIZPL::queryRegPrice(CReg &reg,std::string &outputJson)
{
  std::string res;  
  try{

      reg.checkBanOrReviewOrQuery();
      HolderRaii holderRaii;
      CQIHolder *holder = holderRaii.getMysqlHolder();
      if(-1 == checkGroupIdAndMarket(reg)){
        return -1;
      };
      initMysqlHolder(holder);
      CAuthUser user;
      user.virtualId = reg.virtualId;
      user.authToken = reg.authToken;
      checkAuthToken(holder,user);
      int blcount = 0;
      int isAvailable = 0;
      CDealUnit tmpUnit = (CDealUnit)reg;
      checkBlockLabelAndAvailableType(holder,tmpUnit,blcount,isAvailable);
      reg.price = tmpUnit.price;
      if(0 == blcount||0 == isAvailable)
      {
        throw new CMy_BSDR_Exception("__GLOBAL_BLOCKLABEL_ERR",__GLOBAL_BLOCKLABEL_ERR);
      }

      if(1 == blcount&&1 == isAvailable){

        std::string txid("");
        ndn::CMyMYSQLPL::getInstance()->queryRegPrice(holder,reg,txid);

        res.append("{");
        res.append(writeResultJson("TID",reg.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode","0"));
        res.append(",");
        res.append(writeResultJson("Price",reg.price));
        res.append("}");
        outputJson = res;
        return 0;
        
      }
  }catch(CMy_BSDR_Exception *e)
  {
    res.append("{");
    res.append(writeResultJson("TID",reg.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
    res.append("}");
    outputJson =  res;
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    delete e;
    e = NULL;
    return -1;
  }
    
  return -1; 
}

int ndn::CMyBIZPL::queryRegSum(CReg &reg,std::string &outputJson)
{
    std::string res;
    try{
        reg.checkRegSum();
        HolderRaii holderRaii;
        CQIHolder *holder = holderRaii.getMysqlHolder();
        if(-1 == checkGroupIdAndMarket(reg)){
          return -1;
        };
        initMysqlHolder(holder);

        CAuthUser user;
        user.virtualId = reg.virtualId;
        user.authToken = reg.authToken;
        checkAuthToken(holder,user);
        int resFail = 0;
        int regcount = 0;
        //模糊查询
        resFail = ndn::CMyMYSQLPL::getInstance()->queryRegSum(holder,reg,regcount);
        if(resFail==-1)
        {
          return -1;
        }
        res.append("{");
        res.append(writeResultJson("TID",reg.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode","1"));
        res.append(",");
        res.append(writeResultJson("Sum",std::to_string(regcount)));
        res.append("}");
        outputJson = res;
    }
    catch(CMy_BSDR_Exception *e)
    {
      res.append("{");
      res.append(writeResultJson("TID",reg.tid));
      res.append(",");
      res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
      res.append("}");
      BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
      delete e;
      e = NULL;
      outputJson =  res;
    }
    
    return 0; 
}



//51.市场A查询市场B
//authToken为NULL
int
ndn::CMyBIZPL::queryMarketFromMarket(CPay &cpay,std::string &outputJson){

  std::string res;
  try{
    //
    cpay.checkMarketOrTec();
    HolderRaii holderraii;
    CQIHolder *holder = holderraii.getMysqlHolder();
    initMysqlHolder(holder);
    //这里不需要AuthToken验证
    
    S3FileInfo s3fi;
    s3fi.blocklabel = cpay.blocklabel;
    mainPay(holder,cpay,s3fi);

    res.append("{");
    res.append(writeResultJson("TID",cpay.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode","1"));
    res.append(",");
    res.append(writeResultJson("BlockLabel",cpay.blocklabel));
    res.append(",");
    res.append(writeResultJson("ProviderMarket",cpay.providerMarket));
    res.append(",");
    res.append(writeResultJson("DeviceGroupId",cpay.deviceGroupId));
    res.append(",");
    res.append(writeResultJson("Price",cpay.price));
    res.append(",");
    res.append(writeResultJson("Content",cpay.content));
    res.append(",");
    res.append(writeResultJson("S3Action","4"));
    res.append(",");
    res.append(writeResultJson("S3User",s3fi.s3User));
    res.append(",");
    res.append(writeResultJson("S3Access",s3fi.s3Access));
    res.append(",");
    res.append(writeResultJson("S3Secret",s3fi.s3Secret));
    res.append(",");
    res.append(writeResultJson("S3Host",s3fi.s3Host));
    res.append(",");
    res.append(writeResultJson("S3Bucket",s3fi.s3Bucket));
    res.append(",");
    res.append(writeResultJson("S3DstFile",s3fi.s3DstFile)); 
    res.append(",");
    res.append(writeResultJson("Confirmation","0")); 
    res.append("}");  
    outputJson = res;
    return 0;

  }catch(CMy_BSDR_Exception *e){

    res.append("{");
    res.append(writeResultJson("TID",cpay.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
    res.append(",");
    res.append(writeResultJson("BlockLabel",cpay.blocklabel));
    res.append(",");
    res.append(writeResultJson("ProviderMarket",cpay.providerMarket));
    res.append(",");
    res.append(writeResultJson("DeviceGroupId",cpay.deviceGroupId));
    res.append("}");
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    outputJson =  res;
    delete e;
    e = NULL;
    return -1;
  }
  
  //构建result
  return -1;
}


int
ndn::CMyBIZPL::queryMarketFromUser(CPay &cpay,std::string &outputJson){


  std::string res("");
  try{
    //
    cpay.checkCommon();
    HolderRaii holderraii;
    CQIHolder *holder = holderraii.getMysqlHolder();
    S3FileInfo s3fi;
    s3fi.blocklabel = cpay.blocklabel;
    CAuthUser user;
    user.virtualId = cpay.virtualId;
    user.authToken = cpay.authToken;
    checkAuthToken(holder,user);
    mainPay(holder,cpay,s3fi);

    res.append("{");
    res.append(writeResultJson("TID",cpay.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode","1"));
    res.append(",");
    res.append(writeResultJson("BlockLabel",cpay.blocklabel));
    res.append(",");
    res.append(writeResultJson("ProviderMarket",cpay.providerMarket));
    res.append(",");
    res.append(writeResultJson("DeviceGroupId",cpay.deviceGroupId));
    res.append(",");
    res.append(writeResultJson("Price",cpay.price));
    res.append(",");
    res.append(writeResultJson("Content",cpay.content));
    res.append(",");
    res.append(writeResultJson("S3Action","4"));
    res.append(",");
    res.append(writeResultJson("S3User",s3fi.s3User));
    res.append(",");
    res.append(writeResultJson("S3Access",s3fi.s3Access));
    res.append(",");
    res.append(writeResultJson("S3Secret",s3fi.s3Secret));
    res.append(",");
    res.append(writeResultJson("S3Host",s3fi.s3Host));
    res.append(",");
    res.append(writeResultJson("S3Bucket",s3fi.s3Bucket));
    res.append(",");
    res.append(writeResultJson("S3DstFile",s3fi.s3DstFile)); 
    res.append(",");
    res.append(writeResultJson("Confirmation","0")); 
    res.append("}");  
    outputJson = res;

  }catch(CMy_BSDR_Exception *e)
  {

    res.append("{");
    res.append(writeResultJson("TID",cpay.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
    res.append(",");
    res.append(writeResultJson("BlockLabel",cpay.blocklabel));
    res.append(",");
    res.append(writeResultJson("ProviderMarket",cpay.providerMarket));
    res.append("}"); 
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode()); 
    delete e;
    e = NULL;
    outputJson =  res;
    return -1;
  }

 return 0;
}

//这个都是要经过技术公司确认的 5.3,5.4
int
ndn::CMyBIZPL::queryMarketFromOtherUser(CPay &cpay,std::string &outputJson)
{


  std::string res("");
    try{
      //
      cpay.checkCommon();
      HolderRaii holderraii;
      CQIHolder *holder = holderraii.getMysqlHolder();
      S3FileInfo s3fi;
      s3fi.blocklabel = cpay.blocklabel;
      CAuthUser user;
      user.virtualId = cpay.virtualId;
      user.authToken = cpay.authToken;
      checkAuthToken(holder,user);
      mainPay(holder,cpay,s3fi);

      res.append("{");
      res.append(writeResultJson("TID",cpay.tid));
      res.append(",");
      res.append(writeResultJson("ResultCode","1"));
      res.append(",");
      res.append(writeResultJson("BlockLabel",cpay.blocklabel));
      res.append(",");
      res.append(writeResultJson("ProviderMarket",cpay.providerMarket));
      res.append(",");
      res.append(writeResultJson("DeviceGroupId",cpay.deviceGroupId));
      res.append(",");
      res.append(writeResultJson("Price",cpay.price));
      res.append(",");
      res.append(writeResultJson("Content",cpay.content));
      res.append(",");
      res.append(writeResultJson("S3Action","4"));
      res.append(",");
      res.append(writeResultJson("S3User",s3fi.s3User));
      res.append(",");
      res.append(writeResultJson("S3Access",s3fi.s3Access));
      res.append(",");
      res.append(writeResultJson("S3Secret",s3fi.s3Secret));
      res.append(",");
      res.append(writeResultJson("S3Host",s3fi.s3Host));
      res.append(",");
      res.append(writeResultJson("S3Bucket",s3fi.s3Bucket));
      res.append(",");
      res.append(writeResultJson("S3DstFile",s3fi.s3DstFile)); 
      res.append(",");
      res.append(writeResultJson("Confirmation","1")); 
      res.append("}");  
      outputJson = res;

    }catch(CMy_BSDR_Exception *e)
    {

      res.append("{");
      res.append(writeResultJson("TID",cpay.tid));
      res.append(",");
      res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
      res.append(",");
      res.append(writeResultJson("BlockLabel",cpay.blocklabel));
      res.append(",");
      res.append(writeResultJson("ProviderMarket",cpay.providerMarket));
      res.append(",");
      res.append(writeResultJson("DeviceGroupId",cpay.deviceGroupId));
      res.append("}");
      outputJson =  res;
      BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
      delete e;
      e = NULL;
      return -1;
    }
    return 0;
}

int
ndn::CMyBIZPL::queryMarketFromTecCompany(CPay &cpay,std::string &outputJson)
{
    //authToken为NULL
    queryMarketFromMarket(cpay,outputJson);
    return 0;
}

int
ndn::CMyBIZPL::queryMarketFromTecCompanyAsUser(CPay &cpay,std::string &outputJson)
{
    queryMarketFromUser(cpay,outputJson);
    return 0;
}



int
ndn::CMyBIZPL::queryConsumeSum(CPay &cpay,std::string &outputJson){
  

   std::string sum("");
   std::string res("");
   int resFail = 0;
   int blcount = 0;
   int isAvailable = 0;

   try{

      cpay.checkCommon();
      HolderRaii holderraii;
      CQIHolder *holder = holderraii.getMysqlHolder();
      initMysqlHolder(holder);
      CAuthUser user;
      user.virtualId = cpay.virtualId;
      user.authToken = cpay.authToken;
      checkAuthToken(holder,user);
      checkGroupIdAndMarket(cpay);
      //consumerMarket是否存在
      if(0==marketCount(cpay.consumerMarket)){
       throw new CMy_BSDR_Exception("",__GLOBAL_NO_MARKET_ERR);
      };

      //是否存在blockLabel，是否被禁用
      CDealUnit tmpUnit = (CDealUnit)cpay;
      checkBlockLabelAndAvailableType(holder,tmpUnit,blcount,isAvailable);

      if(1 == blcount&&1 == isAvailable){

          ndn::CMyMYSQLPL::getInstance()->queryConsumeSum(holder,cpay);
      }

      res.append("{");
      res.append(writeResultJson("TID",cpay.tid));
      res.append(",");
      res.append(writeResultJson("ResultCode","1"));
      res.append(",");
      res.append(writeResultJson("Sum",cpay.payMount));
      res.append("}");
      outputJson =  res;
      return 0; 

   }catch(CMy_BSDR_Exception *e)
   {
    res.append("{");
    res.append(writeResultJson("TID",cpay.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
    res.append("}");
    outputJson =  res;
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    delete e;
    e = NULL;
    return -1;
   }

  return -1;
}

int
ndn::CMyBIZPL::queryConsumeBill(CPay &cpay,std::string &outputJson){


  std::string res("");
  int resFail = 0;
  int blcount = 0;
  int isAvailable = 0;

  try{

      cpay.checkCommon();
      HolderRaii holderraii;
      CQIHolder *holder = holderraii.getMysqlHolder();
      initMysqlHolder(holder);
      CAuthUser user;
      user.virtualId = cpay.virtualId;
      user.authToken = cpay.authToken;
      checkAuthToken(holder,user);
     //groupId 与 providerMarket检查
      checkGroupIdAndMarket(cpay);
      //consumerMarket是否存在
      if(0==marketCount(cpay.consumerMarket)){
       throw new CMy_BSDR_Exception("",__GLOBAL_NO_MARKET_ERR);
      };

      //是否存在blockLabel，是否被禁用
      CDealUnit tmpUnit = (CDealUnit)cpay;
      checkBlockLabelAndAvailableType(holder,tmpUnit,blcount,isAvailable);
      if(-1 == resFail){
        return -1;
      }
      if(1 == blcount&&1 == isAvailable){
      
            ndn::CMyMYSQLPL::getInstance()->queryConsumeBill(holder,cpay);
      }

      res.append("{");
      res.append(writeResultJson("TID",cpay.tid));
      res.append(",");
      res.append(writeResultJson("ResultCode","1"));
      res.append(",");
      res.append(writeResultJson("BlockLabel",cpay.blocklabel));
      res.append(",");
      res.append(writeResultJson("ConsumerMarket",cpay.consumerMarket));
      res.append(",");
      res.append(writeResultJson("ProviderMarket",cpay.providerMarket));
      res.append(",");
      res.append(writeResultJson("DeviceGroupId",cpay.deviceGroupId));
      res.append(",");
      res.append(writeResultJson("PayMount",cpay.payMount));
      res.append("}");
      outputJson =  res;

      return 0;
  }
  catch(CMy_BSDR_Exception *e)
  {
    res.append("{");
    res.append(writeResultJson("TID",cpay.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
    res.append("}");
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    delete e;
    e = NULL;
    outputJson =  res;
    return -1;
  }
  
  
  return -1;
}





int ndn::CMyBIZPL::getAuthRandom(CAuthUser &user,std::string &outputJson){

        std::string res("");
        try{
          user.checkGetRamdonRight();
          HolderRaii holderraii;
          CQIHolder *holder = holderraii.getMysqlHolder();
          int resCode = 0;
          std::string res{""};

          CJWTInputIPPort ipport;
          ipport.ip = (Config::Instance()->jwtcfg[0]).ip;
          ipport.port = (Config::Instance()->jwtcfg[0]).port;

          CJWTInputHeader hdr;
          hdr.hdr = "";

          CJWTInputParams params;
          params.user_id = user.virtualId;

          CJWTClient::getInstance()->doJWTComm(JWT_TYPE::JWT_TYPE_AUTHAUTHREQUEST, \
                                                  holder,\
                                                  ipport, \
                                                  hdr, \
                                                  params, \
                                                  resCode, \
                                                  res);

        CJWTResponse jwtres;
        CJWTParser::getInstance()->parseJWTAuthBlob(JWT_TYPE::JWT_TYPE_AUTHAUTHREQUEST, res, jwtres);

        user.ramdon = jwtres.getJWTMessage().message_random.second;

        }catch(CMy_BSDR_Exception *e)
        {
            res = std::string("");
            res.append("{");
            res.append(writeResultJson("TID",user.tid));
            res.append(",");
            res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
            res.append("}");
            BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
            delete e;
            e = NULL;
            outputJson = res;
            return -1;
        }
        res = std::string("");
        res.append("{");
        res.append(writeResultJson("TID",user.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode","1"));
        res.append(",");
        res.append(writeResultJson("Random",user.ramdon));
        res.append("}");
        outputJson = res;


        return 0;
}




int ndn::CMyBIZPL::getAuthTokenfromSig(CAuthUser &user,std::string &outputJson){
  
  std::string res("");
  std::string res1("");
  
  
  try{

    user.checkSignRight();

    HolderRaii holderraii;
    CQIHolder *holder = holderraii.getMysqlHolder();
    
    std::string res("");
    int resCode = 0;


    CJWTInputIPPort ipport;
    ipport.ip = (Config::Instance()->jwtcfg[0]).ip;
    ipport.port = (Config::Instance()->jwtcfg[0]).port;

    CJWTInputHeader hdr;
    hdr.hdr = "";

    CJWTInputParams params;
    params.user_id = user.virtualId;
    params.user_sig = user.sig;

    CJWTClient::getInstance()->doJWTComm(JWT_TYPE::JWT_TYPE_AUTHRESPONSE, \
                                        holder,\
                                        ipport, \
                                        hdr, \
                                        params, \
                                        resCode, \
                                        res);

    //std::cout << "------------------------------------------" << std::endl;
    //std::cout << "resCode = " << resCode << std::endl;
    //std::cout << "response = " << res << std::endl;

    CJWTResponse jwtres;
    CJWTParser::getInstance()->parseJWTAuthBlob(JWT_TYPE::JWT_TYPE_AUTHRESPONSE, res, jwtres);
    //std::cout << "return = " << jwtres.getJWTMessage().message_JWT.second << std::endl;
    user.authToken = jwtres.getJWTMessage().message_JWT.second;

    }catch(CMy_BSDR_Exception *e)
    {
      res1.append("{");
      res1.append(writeResultJson("TID",user.tid));
      res1.append(",");
      res1.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
      res1.append("}");
      BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
      delete e;
      e = NULL;
      outputJson = res;
      return -1;
    }

    res1.append("{");
    res1.append(writeResultJson("TID",user.tid));
    res1.append(",");
    res1.append(writeResultJson("ResultCode","1"));
    res1.append(",");
    res1.append(writeResultJson("AuthToken",user.authToken));

    res1.append("}");
    outputJson = res1;

  return 0;
}




int ndn::CMyBIZPL::reqPubkeySync( CAuthUser &user,std::string &outputJson)
{
  std::string res("");
  try{
    user.checkSynRight();
    HolderRaii holderraii;
    CQIHolder *holder = holderraii.getMysqlHolder();
    initMysqlHolder(holder);
    
    std::string res("");
    int resCode = 0;


    CJWTInputIPPort ipport;
    ipport.ip = (Config::Instance()->jwtcfg[0]).ip;//112
    ipport.port = (Config::Instance()->jwtcfg[0]).port;

    CJWTInputHeader hdr;
    hdr.hdr = "";

    CJWTInputParams params;
    params.user_id = user.virtualId;
    params.user_pubkey = user.userPubKey;

    CJWTClient::getInstance()->doJWTComm(JWT_TYPE::JWT_TYPE_AUTHREGISTER, \
                                        holder,\
                                        ipport, \
                                        hdr, \
                                        params, \
                                        resCode, \
                                        res);

    //std::cout << "------------------------------------------" << std::endl;
    //std::cout << "resCode = " << resCode << std::endl;
    //std::cout << "response = " << res << std::endl;

    CJWTResponse jwtres;
    CJWTParser::getInstance()->parseJWTAuthBlob(JWT_TYPE::JWT_TYPE_AUTHVERIFY, res, jwtres);
    user.userPubKey = jwtres.getJWTResult().result.second;

    res = std::string("");
    res.append("{");
    res.append(writeResultJson("TID",user.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode","1"));
    res.append("}");
    outputJson = res;


  }catch(CMy_BSDR_Exception * e)
  {
    res = std::string("");
    res.append("{");
    res.append(writeResultJson("TID",user.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
    res.append("}");
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    delete e;
    e = NULL;
    outputJson = res;
    return -1;
  }

  return 0;
}


int 
ndn::CMyBIZPL::queryMarketFromBn(CMarket &srcmarket,std::string &outputJson){

    std::string res("");
    CMarket *cm = NULL;

    try{
        
        cm = getMarketFromHashMap(srcmarket.market);
        if(NULL == cm)
        {
          throw new CMy_BSDR_Exception("NOT THIS MARKET",__GLOBAL_NO_MARKET_ERR);
        }
        res.append("{");
        res.append(writeResultJson("TID",srcmarket.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode","1"));
        res.append(",");
        res.append(writeResultJson("IsExist","1"));
        res.append(",");
        res.append(writeResultJson("Balance",std::to_string(cm->balance)));
        res.append("}");
        outputJson = res;

    }catch(CMy_BSDR_Exception *e)
    {
        res.append("{");
        res.append(writeResultJson("TID",srcmarket.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
        res.append(",");
        res.append(writeResultJson("IsExist","0"));
        res.append("}");
        BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
        outputJson = res;

        delete e;
        e = NULL;
        return -1;
    }
    return 0;

}



int 
ndn::CMyBIZPL::mainUpdateRegPrice(CReg &reg,std::string &outputJson)
{

  int blcount = 0;
  int isAvailable= 0;
  int resFail = 0;
  S3FileInfo s3fi;
  BN_LOG_DEBUG("begin updateRegPrice");
  try{
    reg.checkUpdatePrice();
    
    HolderRaii holderraii;
    CQIHolder *holder = holderraii.getMysqlHolder();
    initMysqlHolder(holder);
    CAuthUser user;
    user.virtualId = reg.virtualId;
    user.authToken = reg.authToken; 
    checkGroupIdAndMarket(reg);     
    //todo这里应该返回状态码
    //不存在blocklabel或者blocklabel
    CDealUnit tmpUnit = (CDealUnit)reg;
    checkBlockLabelAndAvailableType(holder,tmpUnit,blcount,isAvailable);
 
    if(1 == blcount&& 1 == isAvailable){
      resFail = ndn::CMyMYSQLPL::getInstance()->updateRegPrice(holder,reg.blocklabel,reg.price,reg.verification,reg.chainId);
    }else{
      
      BN_LOG_ERROR("can not update price");
      BN_LOG_ERROR("can not update blocklabel... blcount:%d%s%d",blcount," isAvailable:",isAvailable);
      throw new CMy_BSDR_Exception("__GLOBAL_BLOCKLABEL_ERR",__GLOBAL_BLOCKLABEL_ERR);
    }

    std::string res("");
    res.append("{");
    res.append(writeResultJson("TID",reg.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode","1"));
    res.append("}");
    outputJson = res;
    
    ///doAfterReg(reg,s3fi);
    
    return 0;

  }catch(CMy_BSDR_Exception *e)
  {
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    std::string res("");
    res.append("{");
    res.append(writeResultJson("TID",reg.tid));
    res.append(",");
    res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
    res.append("}");
    outputJson = res;
    delete e;
    e = NULL;
    return -1;
    
  }

}



//added by liangyuanting
int 
ndn::CMyBIZPL::insertMarketInfo(CMarket &cm,std::string &outputJson){
      //查看时候有
      std::string res("");
      try{

        cm.checkAddRight();

        HolderRaii holderraii;
        CQIHolder *holder = holderraii.getMysqlHolder();
        initMysqlHolder(holder);

        if((mylocalname != cm.deviceGroupId)||(1 == marketCount(cm.market)))
        {
          BN_LOG_ERROR("the market or devicegroupid no right");
          throw new CMy_BSDR_Exception("__GLOBAL_MARKET_ERR",__GLOBAL_MARKET_ERR);
        }
        ndn::CMyMYSQLPL::getInstance()->insertMarket(holder,cm);
        //将cm转为堆结构
        CMarket *newMarket = NULL;
        try{

          newMarket = new CMarket();
          newMarket->deviceGroupId = cm.deviceGroupId;
          newMarket->market = cm.market;
          newMarket->balance = cm.balance;
          newMarket->servFee = cm.servFee;

        }catch(std::bad_alloc &e){

          throw CMy_BSDR_Exception("__GLOBAL_NOT_ALLOC_ERR",__GLOBAL_NOT_ALLOC_ERR);

        }
        putMarketToHashMap(cm.market,newMarket);
        //发送curl命令
        
        res.append("{");
        res.append(writeResultJson("TID",cm.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode","1"));
        res.append("}");
        outputJson = res;
        return 0;

      }catch(CMy_BSDR_Exception *e)
      {

        res.append("{");
        res.append(writeResultJson("TID",cm.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
        res.append("}");
        outputJson = res;
        delete e;
        e = NULL;
        return -1;
      }
       
    
};


int 
ndn::CMyBIZPL::deleteMarketInfo(CMarket &cm,std::string &outputJson){
      
      std::string res("");
      try{
          cm.checkDeleteRight();

          HolderRaii holderraii;
          CQIHolder *holder = holderraii.getMysqlHolder();
          initMysqlHolder(holder);
          
          if((mylocalname != cm.deviceGroupId)||(0 == marketCount(cm.market)))
          {
            BN_LOG_ERROR("the market has been not existed");
            throw new CMy_BSDR_Exception("",__GLOBAL_NO_MARKET_ERR);
          }
          ndn::CMyMYSQLPL::getInstance()->deleteMarket(holder,cm.market);
          deleteMarket(cm.market);
                
          res.append("{");
          res.append(writeResultJson("TID",cm.tid));
          res.append(",");
          res.append(writeResultJson("ResultCode","1"));
          res.append("}");
          outputJson = res;

          return 0;
      }catch(CMy_BSDR_Exception *e)
      {
        res.append("{");
        res.append(writeResultJson("TID",cm.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
        res.append("}");
        outputJson = res;
        delete e;
        e = NULL;
        return -1;
      }
};



int 
ndn::CMyBIZPL::changeMarketBalance(CMarket &cm,std::string &outputJson)
{
  std::string res("");
  try{

              
        HolderRaii holderraii;
        CQIHolder *holder = holderraii.getMysqlHolder();
        initMysqlHolder(holder);
        updateMarketInfo(holder,cm);

        res.append("{");
        res.append(writeResultJson("TID",cm.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode","1"));
        res.append("}");
        outputJson = res;
        return 0;
      }catch(CMy_BSDR_Exception *e)
      {
        res.append("{");
        res.append(writeResultJson("TID",cm.tid));
        res.append(",");
        res.append(writeResultJson("ResultCode",std::to_string(e->getResultCode())));
        res.append("}");
        outputJson = res;
        delete e;
        e = NULL;
        return -1;
      }
}