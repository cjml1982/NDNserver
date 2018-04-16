//Copyright 2018 YiLian Inc.
//Author: liulijin liangyuanting
//this is the function about manager the RegData,and MarketData
#ifndef _MYWALLET_H
#define _MYWALLET_H
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include "myholder.hpp"
#include "tinyxml/tinyxml.h"
#include "constant/myconstant.hpp"
#include "config.h"
#include "mysql/myinstqueue.hpp"
#include "mybasebiz.h"
#ifdef __cplusplus
extern "C" {
#endif
namespace ndn{

class CMyWallet : public CMyBaseBIZ{
public:
    CMyWallet(){
        // MYSQL_IP = __MYSQL_IP;
        MYSQL_USER = __MYSQL_USER;
        MYSQL_PASSWORD = __MYSQL_PASSWORD;
        MYSQL_DATABASE = __MYSQL_DATABASE;

    }

public:

    static CMyWallet* getInstance();
private:
    static CMyWallet *instance;
    // const char * MYSQL_IP;
    const char * MYSQL_USER;
    const char * MYSQL_PASSWORD;
    const char * MYSQL_DATABASE;

public:


    int 
    doCheckReg(CQIHolder * holder,int &blcount,int &isAvailable,CReg &reg);
    
    inline void 
    doAfterReg(CReg &reg,S3FileInfo &s3fi){
    
       
        RegChainInfo hs;
        if(-1 == reg.ackey)
        {
            reg.ackey = getACKey(reg.blocklabel); 
        }
        hs.tid = reg.tid;
        hs.ackey = reg.ackey;
        hs.blocklabel = reg.blocklabel;
        hs.verification = reg.verification;
        hs.market = reg.providerMarket;
        hs.content = reg.content;
        
        //看看是否要放置s3file
        if(!s3fi.blocklabel.empty())
        {
            hs.s3Secret = s3fi.s3Secret;
            hs.s3Access = s3fi.s3Access;
            hs.s3User = s3fi.s3User;
            hs.s3Bucket = s3fi.s3Bucket;
            hs.s3Action = s3fi.s3Action;
            hs.s3DstFile = s3fi.s3DstFile;
            hs.s3Host = s3fi.s3Host;
            hs.s3SrcFileName = s3fi.s3SrcFileName;
            hs.digestf = s3fi.digestf;
        }
        writeChainQueueElement(hs);
    }




    
public:


    int 
    mainRegSet(CQIHolder * holder, CReg &reg);

    int 
    mainRegUpdate(CQIHolder * holder, CReg &reg);

    int 
    mainRegBan(CQIHolder * holder, CReg &reg);

    // int
    // mainUpdateRegPrice(CQIHolder * holder,CReg &reg);
    //end Demo function by liangyt






};


}
    
#ifdef __cplusplus
}
#endif



#endif
