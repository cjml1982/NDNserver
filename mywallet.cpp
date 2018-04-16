#include "mywallet.h"
#include "logging4.h"
#include "openssl/myopenssl.hpp"
#include "constant/myconstant.hpp"
#include "s3file.h"
#include "mybiz.hpp"

#include "mybizpl.hpp"
#include "curl/mycurlpl.hpp"
// #include "json/myjson.hpp"
#include "mysql/mymysqlpl.hpp"


INIT_LOGGER("CMyWallet");


ndn::CMyWallet * ndn::CMyWallet::instance = new CMyWallet();

ndn::CMyWallet * ndn::CMyWallet::getInstance(){
    return instance;
}
//add by liangyuanting
int 
ndn::CMyWallet::mainRegSet(CQIHolder * holder, CReg &reg){

  int blcount = 0;
  int isAvailable= 0;
  int resFail = 0;
  S3FileInfo s3fi;

  BN_LOG_DEBUG("begin to set Reg");
  
  
  try{
      reg.checkAddOrUpdate();
      doCheckReg(holder,blcount,isAvailable,reg);

      //当没有srcFile的时候 表示更改一个表就可以了
      if(1 == blcount)
      {
        throw new CMy_BSDR_Exception("__GLOBAL_DB_EXIST_ITEM_ERR",__GLOBAL_DB_EXIST_ITEM_ERR);
      }
      if(reg.s3srcFileName.empty())
      {
        if(0 == blcount){
            //尚未上链的要update 其有可能是之前未上链的数据 不管怎样 将其丢弃
            resFail = ndn::CMyMYSQLPL::getInstance()->doDataRegAdd(holder,reg);
          }
        
      }
      else {

        if(0 == blcount){
            //尚未上链的要update 其有可能是之前未上链的数据 不管怎样 将其丢弃
            //对两个表进行操作
            //此时表示bl真的是新的
         
              S3File s3file;
              std::string result;
              S3FileInfo s3fi1;
          
              if(200!=s3file.putFile(reg.s3srcFileName,reg.s3srcFileSize,result))
              {
                throw new CMy_BSDR_Exception("__GLOBAL_RESULT_S3_FILE_PUT",__GLOBAL_RESULT_S3_FILE_PUT);
              }

              //对result解析
              s3fi.s3SrcFileName = reg.s3srcFileName;
              s3fi.blocklabel = reg.blocklabel;
              readS3JSON(result,s3fi);

              //增加
              ndn::CMyMYSQLPL::getInstance()->doInsertDataAndFile(holder,reg,s3fi);
              
          }

      }

      if(!s3fi.s3SrcFileName.empty()){
          s3fi.s3Action = "1";  
          postRegFxServ(holder,fxUrl,fxHeader,reg.tid,s3fi);
      }
      else{
        s3fi.s3Action = "0";
      }

   
      doAfterReg(reg,s3fi);
      BN_LOG_DEBUG("end to add Reg");
     
  }catch(CMy_BSDR_Exception *e)
  {
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    //
    try{
        postChainInfoFxServ(holder,fxUrl,fxHeader,reg.tid,std::to_string(e->getResultCode()));
    }catch(CMy_BSDR_Exception *e2)
    {
        delete e2;
        e2 = NULL;
    }
    
    delete e;
    e = NULL;

  }
  
  return 0;
};

int 
ndn::CMyWallet::mainRegUpdate(CQIHolder * holder, CReg &reg){


  int blcount = 0;
  int isAvailable= 0;
  int resFail = 0;
  S3FileInfo s3fi;
  std::string res("");
  BN_LOG_DEBUG("begin to update Reg");

  try{
        reg.checkAddOrUpdate();
        doCheckReg(holder,blcount,isAvailable,reg);

        if(1==blcount&&1==isAvailable)
        {
          if(reg.s3srcFileName.empty())
          {
              S3FileInfo s3fi;
              s3fi.blocklabel = reg.blocklabel;

              ndn::CMyMYSQLPL::getInstance()->selectS3File(holder,s3fi);
              if(!s3fi.s3SrcFileName.empty())
              {
                //删除之
                S3File sf;
                if(200!=sf.deleteFile(s3fi.s3DstFile,s3fi.s3Bucket))
                {
                  throw new CMy_BSDR_Exception("__GLOBAL_RESULT_S3_FILE_DELETE",__GLOBAL_RESULT_S3_FILE_DELETE);
                }

                //对file表进行删除 并进行data更新
                ndn::CMyMYSQLPL::getInstance()->updateDataAndDeleteFile(holder,reg);
                //
              }
              else{
                //直接更新data表
                ndn::CMyMYSQLPL::getInstance()->doDataRegUpdate(holder,reg);
              }
          }
          else{//s3srcFileName不为空

                S3FileInfo s3fi;
                s3fi.blocklabel = reg.blocklabel;
                ndn::CMyMYSQLPL::getInstance()->selectS3File(holder,s3fi);
                if(!s3fi.s3SrcFileName.empty())
                {
                  //如果相等 update之
                   S3File s3file;
                  //获取相应的bucket...
                  if(200!=s3file.updateFile(reg.s3srcFileName,reg.s3srcFileSize,s3fi.s3DstFile,s3fi.s3Bucket,res))
                  {
                    throw new CMy_BSDR_Exception("__GLOBAL_RESULT_S3_FILE_UPDATE",__GLOBAL_RESULT_S3_FILE_UPDATE);
                  }
                  //对result进行解码
                  S3FileInfo s3fi1;
                  readS3JSON(res,s3fi1);
                  //对file data 表进行更新
                  ndn::CMyMYSQLPL::getInstance()->updateDataAndFile(holder,reg,s3fi1);
                  //
                 }     
                }
        }else{
      
          BN_LOG_ERROR("can not update blocklabel...blcount: %d%s%d ",blcount," isAvailable:",isAvailable);
          throw new CMy_BSDR_Exception("__GLOBAL_BLOCKLABEL_ERR",__GLOBAL_BLOCKLABEL_ERR);
        }

        if(!s3fi.s3SrcFileName.empty()){
            s3fi.s3Action = "2";  
            postRegFxServ(holder,fxUrl,fxHeader,reg.tid,s3fi);
        }else{
            s3fi.s3Action = "0";
        }

        doAfterReg(reg,s3fi);

  }catch(CMy_BSDR_Exception *e)
  {
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    //
    try{
        postChainInfoFxServ(holder,fxUrl,fxHeader,reg.tid,std::to_string(e->getResultCode()));
    }catch(CMy_BSDR_Exception *e2)
    {
        delete e2;
        e2 = NULL;
    }
    
    delete e;
    e = NULL;

  }
  
  BN_LOG_DEBUG("end to update Reg");

  return 0;
};

int 
ndn::CMyWallet::mainRegBan(CQIHolder * holder, CReg &reg){
  
  int blcount = 0;
  int isAvailable= 0;
  int resFail = 0;
  S3FileInfo s3fi;

  BN_LOG_DEBUG("begin to ban Reg");

  try{
        reg.checkBanOrReviewOrQuery();
        doCheckReg(holder,blcount,isAvailable,reg);
 
        if(1==blcount&&1==isAvailable){
          resFail = ndn::CMyMYSQLPL::getInstance()->doDataRegBan(holder,reg);
        }else{
          
          BN_LOG_ERROR("can not ban reg");
          BN_LOG_ERROR("can not ban blocklabel...blcount:%d%s%d ",blcount," isAvailable:",isAvailable);
          throw new CMy_BSDR_Exception("__GLOBAL_BLOCKLABEL_ERR",__GLOBAL_BLOCKLABEL_ERR);
        }
        s3fi.s3Action = "3";
        doAfterReg(reg,s3fi);
  }catch(CMy_BSDR_Exception *e)
  {
    BN_LOG_ERROR("failed for %s%s%d",e->getInfo().c_str(),":",e->getResultCode());
    //
    try{
        postChainInfoFxServ(holder,fxUrl,fxHeader,reg.tid,std::to_string(e->getResultCode()));
    }catch(CMy_BSDR_Exception *e2)
    {
        delete e2;
        e2 = NULL;
    }
    
    delete e;
    e = NULL;

  }

  _LOG_INFO("end to ban Reg");
  return 0;
};


int 
ndn::CMyWallet::doCheckReg(CQIHolder * holder,int &blcount,int &isAvailable,CReg &reg){
        int resFail = 0;

        //michael 20180307 added /授权验证
        /*CJWTInputIPPort jwtipport;
        jwtipport.ip = "172.16.25.115";
        jwtipport.port = "8000";
        CJWTInputHeader jwthdr;
        jwthdr.hdr = "";
        CJWTInputParams jwtparams;
        jwtparams.user_id = reg.virtId;
        jwtparams.user_authtoken = reg.authToken;

        int jwtrescode;
        std::string jwtres;
        CJWTClient::getInstance()->doJWTComm(JWT_TYPE::JWT_TYPE_AUTHVERIFY, \
                                             holder, \
                                             jwtipport,
                                             jwthdr,
                                             jwtparams,
                                             jwtrescode,
                                             jwtres);

        CJWTResponse jwtresponse;
        CJWTParser::getInstance()->parseJWTAuthBlob(JWT_TYPE::JWT_TYPE_AUTHVERIFY, \
                                                    jwtres,
                                                    jwtresponse);*/
        
  //    
        CAuthUser user;
        user.virtualId = reg.virtualId;
        user.authToken = reg.authToken;
        checkAuthToken(holder,user);
        checkGroupIdAndMarket(reg);
        initMysqlHolder(holder);
            
        //todo这里应该返回状态码
        //不存在blocklabel或者blocklabel
        CDealUnit tmpUnit = (CDealUnit)reg;
        checkBlockLabelAndAvailableType(holder,tmpUnit,blcount,isAvailable);
      
        BN_LOG_DEBUG("end doCheck!");
    return 0;
}






//end liangyuanting add

