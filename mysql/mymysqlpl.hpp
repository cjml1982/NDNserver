
#ifndef MYMYSQLPL_HPP
#define MYMYSQLPL_HPP

#include <my_global.h>
#include <mysql.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include "myinstqueue.hpp"
#include "../logging4.h"
#include "../openssl/myopenssl.hpp"
#include "../tinyxml/myxml.hpp"
#include "../curl/mycurlpl.hpp"
#include "../myholder.hpp"
#include "../constant/myconstant.hpp"
#include "../config.h"
//michael 20180314 added
#include "mysqlerrcode.hpp"

#ifdef __cplusplus
extern "C" {
#endif

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMyMySQLplResultException : public CMy_BSDR_Exception
{
public:
  CMyMySQLplResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CMyMySQLplResultException() {};
public:
  std::string
  getInfo(void) override final
  {
    return CMy_BSDR_Exception::getInfo();
  }

  int
  getResultCode(void) override final
  {
    return CMy_BSDR_Exception::getResultCode();
  }

  void
  printToConsole(void) override final
  {
    std::cout << "global result code: " << CMy_BSDR_Exception::getResultCode()
        << "global result info: " << CMy_BSDR_Exception::getInfo() << std::endl;
  }

public:
  ostream & operator<<(ostream & out)
  {
    out << "global result code: " << CMy_BSDR_Exception::getResultCode()
    << "global result info: " << CMy_BSDR_Exception::getInfo();
    return out;
  }
};

using namespace std;

namespace ndn {

class CMyMYSQLPL {

public:

  CMyMYSQLPL() {}
  ~CMyMYSQLPL() {}

  static CMyMYSQLPL *
  getInstance();

public:

  int
  getMYSQLClientVersion();

public:



  MYSQL *
  connectMYSQL(const char * ip,
               const char * usr,
               const char * pswd,
               const char * db);

public:

  int
  execSQL(CQIHolder * holder, const char * sql);

  int
  createDB(CQIHolder * holder, const char * sql);



  int
  createTable(CQIHolder * holder, const char * sql);


  int
  addRecords(CQIHolder * holder, const char * sql);


  int 
  insertS3File(CQIHolder *holder,S3FileInfo &s3fi);

  int 
  selectS3File(CQIHolder *holder,S3FileInfo &s3fi);

  int
  updateS3File(CQIHolder *holder,S3FileInfo &input);
  
public:

  int
  addRecordsTX(CQIHolder * holder,
                CPay &cpay,
                const std::string & txid);

  int
  queryRegRecords(CQIHolder * holder,ndn::CReg creg,ndn::RegResult&rreg);


  int
  getRecordsCount(CQIHolder * holder,unsigned int & r_count,std::vector<string> &vec);


  //begin 20170818 liangyuanting add
  int
  getRecordsCount(CQIHolder * holder, unsigned int & r_count);
  //end  20170818 liangyuanting add

  int
  deleteRecords(CQIHolder * holder, const char * sql);

public:

  int
  pingAlive(CQIHolder * holder);

  int
  pingAliveIfReconn(CQIHolder * holder);

public:
  //add by liangyuanting\

  int
  initMysqlHolder(CQIHolder* holder);

  int 
  queryAfterUpdateDemo(CQIHolder* holder, 
                        const std::string & blocklabel,
                        const std::string & txid,
                        const std::string & verfication,
                        const std::string &chainid);
  int
  updateToChainDataAndFile(CQIHolder* holder,RegChainInfo hs,std::string &sChainId);

  int
  updateToChainData(CQIHolder* holder,RegChainInfo hs,std::string &sChainId);

  int 
  updateDataAndDeleteFile(CQIHolder * holder,
                    const CReg&creg);  
  int
  doInsertDataAndFile(CQIHolder * holder,
                    const CReg&creg,S3FileInfo s3fi);
  int 
  updateDataAndFile(CQIHolder * holder,
                   const CReg&creg,S3FileInfo s3fi);
  
  int 
  rechain(CQIHolder* holder, 
            std::queue<RegChainInfo> &q,
            std::map<std::string,std::string> &buffmap,
            std::queue<S3FileInfo> &qs3fi);
  
  int
  hasblocklabel(CQIHolder* holder,
                CDealUnit &unit,
                int &blockcount,
                std::string &txid,
                int &available);

  //end by liangyuanting

  //add by liangyuanting 
  int
  insertMarket(CQIHolder *holder,CMarket &cm);

  int
  updateMarket(CQIHolder *holder,CMarket *cm);

  int
  deleteMarket(CQIHolder *holder,std::string &mk);

  //登记

  int 
  doDataRegAdd(CQIHolder * holder,
                  const CReg&creg);

  int 
  doDataRegUpdate(CQIHolder * holder,
                  const CReg&creg);
    
  
  int 
  doDataRegBan(CQIHolder * holder,
                  const CReg&creg);

  int 
  regAvailable(CQIHolder *holder,
                    const std::string & blocklabel,
                    const std::string & myTable,
                    int &result);

  int
  updateRegPrice(CQIHolder *holder,
                std::string &blocklabel,
                std::string &price,
                std::string &verification,
                std::string &mytable);
  
  int 
  queryRegSum(CQIHolder *holder,
                CReg &reg,
                int &regcount);
  int 
  queryRegPrice(CQIHolder *holder,
                CReg &reg,
                std::string &txid);

  int
  updateMoney(CQIHolder *holder,
              CMarket *cm1,
              CMarket *cm2);   


  int 
  deleteDataAndFile(CQIHolder *holder,std::string blocklabel,std::string sChainId); 

  int 
  deleteData(CQIHolder *holder,std::string blocklabel,std::string sChainId); 

  
  inline void
  throwQueryException(MYSQL * client)
  {
    switch ((MYSQL_ERR_CODE)mysql_errno(client))
    {
      case MYSQL_ERR_CODE::MY_MYSQL_UNKNOWN_ERROR:
        throw new CMyMySQLplResultException{"query err", __GLOBAL_MY_MYSQL_UNKNOWN_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SERVER_GONE_ERROR:
        throw new CMyMySQLplResultException{"query err", __GLOBAL_MY_MYSQL_SERVER_GONE_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_COMMANDS_OUT_OF_SYNC:
        throw new CMyMySQLplResultException{"query err", __GLOBAL_MY_MYSQL_COMMANDS_OUT_OF_SYNC};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SERVER_LOST:
        throw new CMyMySQLplResultException{"query err", __GLOBAL_MY_MYSQL_SERVER_LOST};
        break;
      default: break;
    }
  }

  int 
  queryConsumeSum(CQIHolder *holder,CPay &cpay);

  int 
  queryConsumeBill(CQIHolder *holder,CPay &cpay);
  
private:
  static CMyMYSQLPL * instance;
  std::mutex connLock;
  std::mutex mtx_queue;
  std::mutex mtx_buffmap;
  int  chainTotal;
};

}

#ifdef __cplusplus
}
#endif

#endif

