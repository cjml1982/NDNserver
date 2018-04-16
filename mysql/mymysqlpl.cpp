
#include "mymysqlpl.hpp"

//begin 20170629 liulijin add

INIT_LOGGER("CMyMYSQLPL");

ndn::CMyMYSQLPL * ndn::CMyMYSQLPL::instance = new CMyMYSQLPL;

ndn::CMyMYSQLPL *
ndn::CMyMYSQLPL::getInstance()
{
  return instance;
}


int
ndn::CMyMYSQLPL::getMYSQLClientVersion()
{
  _LOG_INFO("begin getMYSQLClientVersion");

 //fprintf(stderr, "MySQL client version: %s\n", mysql_get_client_info());
  _LOG_INFO("MySQL client version: " << mysql_get_client_info());

  _LOG_INFO("end getMYSQLClientVersion");
  return 0;
}


MYSQL *
ndn::CMyMYSQLPL::connectMYSQL(const char * ip,
                              const char * usr,
                              const char * pswd,
                              const char * db)
{
  _LOG_INFO("begin connectMYSQL");

  int ret = 0;
  ret = mysql_thread_init();
  if (ret != 0)
  {
   //fprintf(stderr, "sql thread init fail!\n");
    _LOG_ERROR("sql thread init fail!");
    mysql_thread_end();
    return NULL;
  }

  //change to the heap struct
  MYSQL * client = NULL;
  try{

      client = new MYSQL();
  }
  catch(std::bad_alloc &e)
  {
    _LOG_ERROR("new operator failed");
  }
 
  mysql_init(client);
  if (client == NULL)
  {
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
    mysql_close(client);
    mysql_thread_end();
    client = NULL;
    return NULL;
  }

  unsigned int connTimeOut = __MYSQL_CONN_TIMEOUT_MILS;
  ret = mysql_options(client, MYSQL_OPT_CONNECT_TIMEOUT, &connTimeOut);
  if (ret != 0)
  {
   //fprintf(stderr, "could not set SQL options\n");
    _LOG_ERROR("could not set SQL options");
    mysql_close(client);
    mysql_thread_end();
    client = NULL;
    return NULL;
  }

  unsigned int port=0;
  const char * unixSock = 0;
  unsigned long flag=0;
  client = mysql_real_connect(client, ip, usr, pswd, db, port, unixSock, flag);
  if (NULL == client)
  {
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
    mysql_close(client);
    mysql_thread_end();
    client = NULL;

    switch ((MYSQL_ERR_CODE)mysql_errno(client))
    {
      case MYSQL_ERR_CODE::MY_MYSQL_CONN_HOST_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_CONN_HOST_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_CONNECTION_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_CONNECTION_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_IPSOCK_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_IPSOCK_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_OUT_OF_MEMORY:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_OUT_OF_MEMORY};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SOCKET_CREATE_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_SOCKET_CREATE_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_UNKNOWN_HOST:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_UNKNOWN_HOST};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_VERSION_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_VERSION_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_NAMEDPIPEOPEN_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_NAMEDPIPEOPEN_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_NAMEDPIPEWAIT_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_NAMEDPIPEWAIT_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_NAMEDPIPESETSTATE_ERROR:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_NAMEDPIPESETSTATE_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SERVER_LOST:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_SERVER_LOST};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_ALREADY_CONNECTED:
        throw new CMyMySQLplResultException{"connect err", __GLOBAL_MY_MYSQL_ALREADY_CONNECTED};
        break;
      default: break;
    }
    return NULL;
  }

  ret = mysql_set_character_set(client, "utf8mb4");
  if (ret != 0)
  {
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
    mysql_close(client);
    mysql_thread_end();
    client = NULL;
    //no throw code could be use.
    return NULL;
  }

  _LOG_INFO("end connectMYSQL");
  return client;
}

int
ndn::CMyMYSQLPL::execSQL(CQIHolder * holder, const char * sql)
{
  _LOG_INFO("begin execSQL");

  int ret = 0;
  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
  if (-1 == ret)
  {
    BN_LOG_ERROR("pingAliveIfReconn next...");
   // throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
   return -1;

  }

  ret = mysql_real_query(holder->client, sql, strlen(sql));
  if (ret != 0)
  {
   //fprintf(stderr, "%s\n", sql);
    _LOG_ERROR("sql=" << sql);
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(holder->client), mysql_error(holder->client));
    BN_LOG_ERROR("Error:%d%s%s",mysql_errno(holder->client) , "," , mysql_error(holder->client));
    if(mysql_errno(holder->client)==2013||mysql_errno(holder->client)==2006)
    {
        mysql_close(holder->client);
        mysql_thread_end();
        for(int i = 0;i<Config::Instance()->dbvec.size();i++)
        {
          
          holder->client = connectMYSQL(((Config::Instance()->dbvec[i]).ip).c_str(), __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
          if(NULL!=holder->client)
          {
            ret = mysql_real_query(holder->client, sql, strlen(sql));
            if(ret != 0)
            {
              BN_LOG_ERROR("re to query error!");
              //  throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
              return -1;
            }
          }
        }
        if (NULL == holder->client)
        {
        //fprintf(stderr, "sql handler is NULL!\n");
          BN_LOG_ERROR("sql handler is NULL!");
        // throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
          return -1;
        } 
        return 0;
      // throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
      
    }



     throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
  }

  _LOG_INFO("end execSQL");
  return 0;
}

int
ndn::CMyMYSQLPL::createDB(CQIHolder * holder, const char * sql)
{
  _LOG_INFO("begin createDB");

  int ret = 0;
  ret = execSQL(holder, sql);
  if (ret == -1)
  {
   //fprintf(stderr, "createDB error!\n");
    BN_LOG_DEBUG("createDB error!");
    return -1;
  }

  _LOG_INFO("end createDB");
  return 0;
}


int
ndn::CMyMYSQLPL::createTable(CQIHolder * holder, const char * sql)
{
  _LOG_INFO("begin createTable");

  int ret = 0;
  ret = execSQL(holder, sql);
  if (ret == -1)
  {
   //fprintf(stderr, "createTable error!\n");
    BN_LOG_DEBUG("createTable error!");
    return -1;
  }

  _LOG_INFO("end createTable");
  return 0;
}


int
ndn::CMyMYSQLPL::addRecords(CQIHolder * holder, const char * sql)
{
  _LOG_INFO("begin addRecords");

  int ret = 0;
  ret = execSQL(holder, sql);
  if (ret == -1)
  {
   //fprintf(stderr, "addRecords error!\n");
    BN_LOG_DEBUG("addRecords error!");
    return -1;
  }

  _LOG_INFO("end addRecords");
  return 0;
}


int
ndn::CMyMYSQLPL::addRecordsTX(CQIHolder * holder,
                              CPay &cpay,
                              const std::string & txid)
{
  _LOG_INFO("begin addRecordsTX");

  std::string sql("");
  sql.append("insert into mymysql.");
  sql.append(__TABLE_TX);
  sql.append(cpay.chainId);
  sql.append(" (blocklabel,market,deviceGroupId,txid,verification,servfee,amount) values ('");
  sql.append(cpay.blocklabel);
  sql.append("','");
  sql.append(cpay.providerMarket);
  sql.append("','");
  sql.append(cpay.deviceGroupId);
  sql.append("','");
  sql.append(txid);
  sql.append("','");
  sql.append(cpay.verification);
  sql.append("','");
  sql.append(cpay.servfee);
  sql.append("','");
  sql.append(cpay.price);
  sql.append("');");
  _PRINT_KEY_VALUE(sql);
  int ret = 0;
  ret = execSQL(holder, sql.c_str());
  // if (ret == -1)
  // {
  //  //fprintf(stderr, "addRecordsTX error!\n");
  //   _LOG_ERROR("addRecordsTX error!\n");
  //   return -1;
  // }

  _LOG_INFO("end addRecordsTX");
  return 0;
}


int
ndn::CMyMYSQLPL::pingAlive(CQIHolder * holder)
{
  _LOG_INFO("begin pingAlive");

  if (NULL == holder->client)
  {
   //fprintf(stderr, "sql handler is NULL!\n");
    _LOG_ERROR("sql handler is NULL!\n");
    throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
  }

  int ret = 0;
  _LOG_INFO("client address is "<<holder->client);
  ret = mysql_ping(holder->client);
  if (ret != 0)
  {
    //fprintf(stderr, "Error %u: %s\n", mysql_errno(holder->client), mysql_error(holder->client));
    _LOG_ERROR("Error:" << mysql_errno(holder->client) << "," << mysql_error(holder->client));
    
  }

  _LOG_INFO("end pingAlive");
  return 0;
}

int
ndn::CMyMYSQLPL::pingAliveIfReconn(CQIHolder * holder)
{

 // _LOG_INFO("begin pingAliveIfReconn");
  if (NULL == holder->client)
  {
   //fprintf(stderr, "sql handler is NULL!\n");
     BN_LOG_ERROR("sql handler is NULL!");

      for( int i = 0;i<Config::Instance()->dbvec.size();i++)
      {
        
        holder->client = connectMYSQL(((Config::Instance()->dbvec[i]).ip).c_str(), __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
        if(NULL!=holder->client)
        {
          break;
        }
      }
    if (NULL == holder->client)
    {
      BN_LOG_ERROR("sql handler is NULL still!");
  
      //throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
      return -1;
    }
  }
  int ret = mysql_ping(holder->client);
  if (ret != 0)
  {
    _LOG_ERROR("Error:" << mysql_errno(holder->client) << "," << mysql_error(holder->client));
    mysql_close(holder->client);
    mysql_thread_end();
    for(int i = 0;i<(Config::Instance()->dbvec.size());i++)
    {
      holder->client = connectMYSQL(((Config::Instance()->dbvec[i]).ip).c_str(), __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
      if (NULL != holder->client)
      {
        break;
      } 
    }
    if (NULL == holder->client)
    {
      BN_LOG_ERROR("sql handler is NULL!");
      //throw new CMy_BSDR_Exception(std::string(mysql_error(holder->client)),(int)mysql_errno(holder->client)+10000);
      return -1;
    }
  }
  _LOG_INFO("end pingAliveIfReconn");
  return 0;
}





//the new model
int
ndn::CMyMYSQLPL::updateToChainDataAndFile(CQIHolder* holder, 
                                    RegChainInfo hs,
                                    std::string & chainid)
{
BN_LOG_DEBUG("begin updateToChainDataAndFile");
  //MYSQL* client = holder->client;

  //修改data表
  int ret = 0;
  std::string sql1("update mymysql.");
  sql1.append(__TABLE_DATA);
  sql1.append(chainid);
  sql1.append(" set txid='");
  sql1.append(hs.txid);
  sql1.append("', verification = '");
  sql1.append(hs.verification);
  sql1.append("',txidstate = 2 where blocklabel='");
  sql1.append(hs.blocklabel);
  sql1.append("';");
  _PRINT_KEY_VALUE(sql1);


  std::string sql2("update mymysql.file");
  sql2.append(chainid);
  sql2.append(" set digestf ='");
  sql2.append(hs.digestf);
  sql2.append("',uploadstatus = 1 where blocklabel='");
  sql2.append(hs.blocklabel);
  sql2.append("';");
  _PRINT_KEY_VALUE(sql2);


    int ON = 1;
  	int OFF = 0;
    int i = 0;
    int errorno = 0;
    std::string errormsg = "";

    pingAliveIfReconn(holder);
  
    mysql_autocommit(holder->client,OFF);
    mysql_autocommit(holder->client,OFF);

   
    if(mysql_query(holder->client,sql1.c_str()))
    {
      errorno = mysql_errno(holder->client);
      errormsg = mysql_error(holder->client);
      BN_LOG_ERROR("error sql:%s",sql1.c_str());
      i = 1;
    }
    if(mysql_query(holder->client,sql2.c_str()))
    {
      BN_LOG_ERROR("error sql:%s",sql2.c_str());
      i = 1;
    }
    if(i == 1)
    {
      mysql_rollback(holder->client);
    }
    else
    {
      mysql_commit(holder->client);		
    }
    mysql_autocommit(holder->client,ON);
    if(1 == i)
    {
      throw new CMy_BSDR_Exception(errormsg.c_str(),errorno+10000);
    }
 
  BN_LOG_DEBUG("updateToChainDataAndFile");
  return 0;
}


int
ndn::CMyMYSQLPL::updateToChainData(CQIHolder* holder, 
                                    RegChainInfo hs,
                                    std::string & chainid)
{
  BN_LOG_DEBUG("begin updateToChainData");
  //MYSQL* client = holder->client;

  //修改data表
  int ret = 0;
  std::string iSQL("update mymysql.");
  iSQL.append(__TABLE_DATA);
  iSQL.append(chainid);
  iSQL.append(" set txid='");
  iSQL.append(hs.txid);
  iSQL.append("', verification = '");
  iSQL.append(hs.verification);
  iSQL.append("',txidstate = 2 where blocklabel='");
  iSQL.append(hs.blocklabel);
  iSQL.append("';");
  _PRINT_KEY_VALUE(iSQL);
  execSQL(holder,iSQL.c_str());
  BN_LOG_DEBUG("end queryAfterAdd");
  return 0;
}

//涉及到上链数据的处理
int ndn::CMyMYSQLPL::rechain(CQIHolder* holder, std::queue<ndn::RegChainInfo> &q,std::map<std::string, std::string > &buffmap,std::queue<S3FileInfo> &qs3fi)
{
      chainTotal = Config::Instance()->portVec.size();
      MYSQL* client = holder->client;
      MYSQL_RES * cursor;
      MYSQL_FIELD * fieldtitle;
      MYSQL_ROW records;
      std::string sql("");
      for(int i = 0;i<chainTotal-1;i++)
      {
        sql.append("select blocklabel,verification,market,tid,content from mymysql.");
        sql.append(__TABLE_DATA);
        sql.append(std::to_string(i));
        sql.append(" where txidstate = 1");
        sql.append(" union ");
      }
      sql.append("select blocklabel,verification,market,tid,content from mymysql.");
      sql.append(__TABLE_DATA);
      sql.append(std::to_string(chainTotal-1));
      sql.append(" where  txidstate = 1;");
      _PRINT_KEY_VALUE(sql);

      int ret = 0;
      ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);

      ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
      if (ret != 0)
      {
        //_LOG_INFO("sql=" << sql);
         
        BN_LOG_ERROR("Error:%d%s%s",mysql_errno(client) , "," , mysql_error(client));
        throwQueryException(client);
       
      }
    
      cursor = mysql_store_result(client);
      if (cursor)
      {
        // while ((fieldtitle = mysql_fetch_field(cursor)))
        // {
        //   _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
        // }
        while ((records = mysql_fetch_row(cursor)))
        {
            //_LOG_INFO("records=" << records[0]);
            std::lock_guard<std::mutex> lg(mtx_queue);
            RegChainInfo hs; 
            hs.blocklabel = std::string(records[0]);
            hs.verification = std::string(records[1]);
            hs.market = std::string(records[2]);
            hs.tid = std::string(records[3]);
            hs.content = std::string(records[4]);
            //对每个东西
            S3FileInfo s3fi;
            s3fi.blocklabel = hs.blocklabel;
            selectS3File(holder,s3fi);
            //组建新的hs
            //如果s3有内容
            if(!s3fi.s3SrcFileName.empty())
            {
                  hs.s3Secret = s3fi.s3Secret;
                  hs.s3Access = s3fi.s3Access;
                  hs.s3Bucket = s3fi.s3Bucket;
                  s3fi.tid = hs.tid;
                  //hs.s3Action = s3fi.s3Action;
                  hs.s3DstFile = s3fi.s3DstFile;
                  hs.s3Host = s3fi.s3Host;
                  hs.s3SrcFileName = s3fi.s3SrcFileName;
                  if(!s3fi.digestf.empty())
                  {
                    std::lock_guard<std::mutex> lg(mtx_buffmap);
                    buffmap[hs.blocklabel] = s3fi.digestf;
                    hs.digestf = s3fi.digestf;
                    //默认为更新
                    s3fi.s3Action = "2";
                  }
                  else{//这时 你不知道 是不是已将s3的url发送 

                    s3fi.s3Action = "1";
                    
                  }
                  qs3fi.push(s3fi);

            }

            q.push(hs);
            
          
        }
        mysql_free_result(cursor);
        while (!mysql_next_result(client))
        {
          cursor = mysql_store_result(client);
          mysql_free_result(cursor);
        }
      }
}


int 
ndn::CMyMYSQLPL::hasblocklabel(CQIHolder* holder,
                                    CDealUnit &unit,
                                    int &blockcount,
                                    std::string &txid,
                                    int &available)
{
  int ret = 0;
  MYSQL *client = holder->client;
  MYSQL_RES * cursor;
  MYSQL_FIELD * fieldtitle;
  MYSQL_ROW records;
  /*
  1." " 123 此时blockcount为-1 尚未上链
  2.empty 此时为 0 不存在该链
  3 "sd" 

  */
  std::string sql("select txid,content,available,verification,price,txidstate from mymysql.");
  sql.append(__TABLE_DATA);
  sql.append(unit.chainId);
  sql.append(" where blocklabel='");
  sql.append(unit.blocklabel);
  sql.append("' and market = '");
  sql.append(unit.providerMarket);
  // sql.append("' and txidstate = ");
  // sql.append(std::to_string(2));
  sql.append("' and devicegroupid = '");
  sql.append(unit.deviceGroupId);
  sql.append("'");
 
  _PRINT_KEY_VALUE(sql);
  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
  if (-1 == ret)
  {
    BN_LOG_ERROR("pingAliveIfReconn next...");
    return -1;
  }

  ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
  if (ret != 0)
  {
    _LOG_ERROR("sql=" << sql);
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));

    throwQueryException(client);
 
  }
  cursor = mysql_store_result(client);
  if (cursor)
  {
    while ((fieldtitle = mysql_fetch_field(cursor)))
    {
      _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
    }
    while ((records = mysql_fetch_row(cursor)))
    {
      _LOG_INFO("records=" << records[0]);
      //txid可能为null

      txid = std::string(records[0]);
      blockcount = 1;
      unit.content = std::string(records[1]);
      available = atoi(records[2]);
      unit.verification = std::string(records[3]);
      unit.price =  std::string(records[4]);
     if("1" == std::string(records[5]))
          throw new CMy_BSDR_Exception("__GLOBAL_EXIST_NO_CHAIN_RECORD_ERR",__GLOBAL_EXIST_NO_CHAIN_RECORD_ERR);
    }
    mysql_free_result(cursor);
    while (!mysql_next_result(client))
    {
      cursor = mysql_store_result(client);
      mysql_free_result(cursor);
    }
  }
  return 0;
};



int
ndn::CMyMYSQLPL::insertMarket(CQIHolder *holder,CMarket &cm){
  
    _LOG_INFO("begin insertMarket");

    std::string sql("");
    sql.append("insert into mymysql.market (devicegroupid,market,balance) values('");
    sql.append(cm.deviceGroupId);
    sql.append("','");
    sql.append(cm.market);
    sql.append("','");
    sql.append(std::to_string(cm.balance));
    sql.append("');");
    int ret = 0;
    _PRINT_KEY_VALUE(sql);
    ret = execSQL(holder, sql.c_str());
    
    if (ret == -1)
    {
      _LOG_INFO("insertMarket error!\n");
      return -1;
    }

    _LOG_INFO("end insertMarket");

    return 0;
};



int 
ndn::CMyMYSQLPL::deleteDataAndFile(CQIHolder *holder,
                                    std::string blocklabel,
                                    std::string sChainId){
      
      std::string sql1("");
      sql1.append("delete from mymysql.data");
      sql1.append(sChainId);
      sql1.append(" where blocklabel = '");
      sql1.append(blocklabel);
      sql1.append("';");

      std::string sql2("");
      sql2.append("delete from mymysql.file");
      sql2.append(" where blocklabel = '");
      sql2.append(blocklabel);
      sql2.append("';");


      int ON = 1;
      int OFF = 0;
      int i = 0;
      int errorno = 0;
      std::string errormsg = "";
      pingAliveIfReconn(holder);
      mysql_autocommit(holder->client,OFF);
      mysql_autocommit(holder->client,OFF);

      int ret = 0;
      
      if(mysql_query(holder->client,sql1.c_str()))
      {
        errorno = mysql_errno(holder->client);
        errormsg = mysql_error(holder->client);
        BN_LOG_ERROR("error sql:%s",sql1.c_str());
        i = 1;
      }
      if(mysql_query(holder->client,sql2.c_str()))
      {
        BN_LOG_ERROR("error sql:%s",sql2.c_str());
        i = 1;
      }
      if(i == 1)
      {
        mysql_rollback(holder->client);
      }
      else
      {
        mysql_commit(holder->client);		
      }
      mysql_autocommit(holder->client,ON);
      if(1 == i)
      {
        throw new CMy_BSDR_Exception(errormsg.c_str(),errorno+10000);
      }
      BN_LOG_DEBUG("end delete Data and File");

      return 0;
  }; 


int 
ndn::CMyMYSQLPL::deleteData(CQIHolder *holder,
                                    std::string blocklabel,
                                    std::string sChainId){
      
      std::string sql1("");
      sql1.append("delete from mymysql.data");
      sql1.append(sChainId);
      sql1.append(" where blocklabel = '");
      sql1.append(blocklabel);
      sql1.append("';");
      pingAliveIfReconn(holder);
      execSQL(holder,sql1.c_str());
      
      BN_LOG_DEBUG("end deleteData");
      return 0;
  }; 

int
ndn::CMyMYSQLPL::updateMoney(CQIHolder *holder,CMarket *cm1,CMarket *cm2){

    BN_LOG_DEBUG("begin updateMoney");
    
    std::string sql1("");
    sql1.append("update mymysql.market set ");
    sql1.append(" balance='");
    sql1.append(std::to_string(cm1->balance));
    sql1.append("' where market='");
    sql1.append(cm1->market);
    sql1.append("' and deviceGroupid = '");
    sql1.append(cm1->deviceGroupId);
    sql1.append("';");



    std::string sql2("");
    sql2.append("update mymysql.market set ");
    sql2.append(" balance='");
    sql2.append(std::to_string(cm2->balance));
    sql2.append("' where market='");
    sql2.append(cm2->market);
    sql2.append("' and deviceGroupid = '");
    sql2.append(cm2->deviceGroupId);
    sql2.append("';");


    int ON = 1;
  	int OFF = 0;
    int i = 0;
    int errorno = 0;
    std::string errormsg = "";

    pingAliveIfReconn(holder);
  
    mysql_autocommit(holder->client,OFF);
    mysql_autocommit(holder->client,OFF);

    int ret = 0;
    if(mysql_query(holder->client,sql1.c_str()))
    {
      errorno = mysql_errno(holder->client);
      errormsg = mysql_error(holder->client);
      BN_LOG_ERROR("error sql:%s",sql1.c_str());
      i = 1;
    }
    if(mysql_query(holder->client,sql2.c_str()))
    {
      BN_LOG_ERROR("error sql:%s",sql2.c_str());
      i = 1;
    }
    if(i == 1)
    {
      mysql_rollback(holder->client);
    }
    else
    {
      mysql_commit(holder->client);		
    }
    mysql_autocommit(holder->client,ON);
    if(1 == i)
    {
      throw new CMy_BSDR_Exception(errormsg.c_str(),errorno+10000);
    }
    BN_LOG_DEBUG("end updateMoney");

  return 0;
};


int
ndn::CMyMYSQLPL::updateMarket(CQIHolder *holder,CMarket *cm){

    _LOG_INFO("begin updateMarket");
    
    std::string sql("");
    sql.append("update mymysql.market set devicegroupid='");
    sql.append(cm->deviceGroupId);
    sql.append("',market='");
    sql.append(cm->market);
    // sql.append("',servFee='");
    // sql.append(cm->servFee);
    sql.append("',balance='");
    sql.append(std::to_string(cm->balance));
    sql.append("' where market='");
    sql.append(cm->market);
    sql.append("';");    
    int ret = 0;
    _PRINT_KEY_VALUE(sql);
    ret = execSQL(holder, sql.c_str());
    
    if (ret == -1)
    {
      _LOG_ERROR("updateMarket error!\n");
      return -1;
    }

    _LOG_INFO("end updateMarket");


  return 0;
};

int
ndn::CMyMYSQLPL::deleteMarket(CQIHolder *holder,std::string &mk){

  std::string sql("");
  sql.append("delete from mymysql.market where market = '");
  sql.append(mk);
  sql.append("';");

  int ret = 0;
  ret = execSQL(holder, sql.c_str());

  if (ret == -1)
  {
    _LOG_ERROR("deleteMarket error!\n");
    return -1;
  }

  _LOG_ERROR("end deleteMarket");

  return 0;
};



 int 
 ndn::CMyMYSQLPL::updateDataAndFile(CQIHolder * holder,
                  const CReg&creg,S3FileInfo s3fi )
{
   std::string sql1("");
    sql1.append("update mymysql.");
    sql1.append(__TABLE_DATA);
    sql1.append(creg.chainId);
    sql1.append(" set verification = '");
    sql1.append(creg.verification);
    sql1.append("', version = version+1,content = '");
    sql1.append(creg.content);
    sql1.append("',tid = '");
    sql1.append(creg.tid);
    sql1.append("',price = '");
    sql1.append(creg.price);
    sql1.append("',servfee = '");
    sql1.append(creg.servfee);
    sql1.append("',available = ");
    sql1.append("1");
    sql1.append(",txidstate = 1");
    sql1.append(",stamp = ");
    sql1.append("now()");
    sql1.append(" where blocklabel = '");
    sql1.append(creg.blocklabel);
    sql1.append("' and market = '");
    sql1.append(creg.providerMarket);
    sql1.append("' and devicegroupid = '");
    sql1.append(creg.deviceGroupId);
    sql1.append("';");

    std::string sql2 = std::string("update mymysql.file");
    sql2.append(" set s3host = '");
    sql2.append(s3fi.s3Host);
    sql2.append("',s3access = '");
    sql2.append(s3fi.s3Access);
    sql2.append("',s3Bucket = '");
    sql2.append(s3fi.s3Bucket);
    sql2.append("',s3DstFile = '");
    sql2.append(s3fi.s3DstFile);
    sql2.append("',s3Secret = '");
    sql2.append(s3fi.s3Secret);
    sql2.append("',s3SrcFileName = '");
    sql2.append(s3fi.s3SrcFileName);
    sql2.append("',digestf = '");
    sql2.append(s3fi.digestf);
    sql2.append("',s3SrcFileSize = '");
    sql2.append(std::to_string(creg.s3srcFileSize));
    sql2.append("',s3User = '");
    sql2.append(s3fi.s3User);
    sql2.append("' where blocklabel = '");
    sql2.append(s3fi.blocklabel);
    sql2.append("';");

     int ON = 1;
    int OFF = 0;
    int i = 0;
    int errorno = 0;
    std::string errormsg = "";

    pingAliveIfReconn(holder);

    mysql_autocommit(holder->client,OFF);
    mysql_autocommit(holder->client,OFF);

    int ret = 0;
    if(mysql_query(holder->client,sql1.c_str()))
    {
      errorno = mysql_errno(holder->client);
      errormsg = mysql_error(holder->client);
      BN_LOG_ERROR("error sql:%s",sql1.c_str());
      i = 1;
    }
    if(mysql_query(holder->client,sql2.c_str()))
    {
      BN_LOG_ERROR("error sql:%s",sql2.c_str());
      i = 1;
    }
    if(i == 1)
    {
      mysql_rollback(holder->client);
    }
    else
    {
      mysql_commit(holder->client);		
    }
    mysql_autocommit(holder->client,ON);
    if(1 == i)
    {
      throw new CMy_BSDR_Exception(errormsg.c_str(),errorno+10000);
    }
 
  
  return 0;
}
        
 int 
 ndn::CMyMYSQLPL::doInsertDataAndFile(CQIHolder * holder,
                  const CReg&creg,S3FileInfo s3fi )
 {

  std::string sql1("");
  sql1.append("insert into mymysql.");
  sql1.append(__TABLE_DATA);
  sql1.append(creg.chainId);
  sql1.append(" (tid,blocklabel,market,"
             "devicegroupid,verification,"
             "servfee,price,available,"
             "version,stamp,content,txidstate) values ('");
  sql1.append(creg.tid);
  sql1.append("','");
  sql1.append(creg.blocklabel);
  sql1.append("','");
  sql1.append(creg.providerMarket);
  sql1.append("','");
  sql1.append(creg.deviceGroupId);
  sql1.append("','");
  sql1.append(creg.verification);
  sql1.append("','");
  sql1.append(creg.servfee);
  sql1.append("','");
  sql1.append(creg.price);
  sql1.append("','");
  sql1.append("1");
  sql1.append("','");
  sql1.append("1");
  sql1.append("',");
  sql1.append("now(),'");
  sql1.append(creg.content);
  sql1.append("',");
  sql1.append("1");
  sql1.append(");");


  std::string sql2("");
  sql2.append("insert into mymysql.file ( blocklabel,s3host,s3access,s3secret,s3bucket,s3srcfilename,s3srcfilesize,s3dstfile,s3user ) values ('");
  sql2.append(creg.blocklabel);
  sql2.append("','");
  sql2.append(s3fi.s3Host);
  sql2.append("','");
  sql2.append(s3fi.s3Access);
  sql2.append("','");
  sql2.append(s3fi.s3Secret);
  sql2.append("','");
  sql2.append(s3fi.s3Bucket);
  sql2.append("','");
  sql2.append(s3fi.s3SrcFileName);
  sql2.append("','");
  sql2.append(std::to_string(creg.s3srcFileSize));
  sql2.append("','");
  sql2.append(s3fi.s3DstFile);
  sql2.append("','");
  sql2.append(s3fi.s3User);
  sql2.append("');");

  int ON = 1;
  int OFF = 0;
  int i = 0;
  int errorno = 0;
  std::string errormsg = "";

  pingAliveIfReconn(holder);

  mysql_autocommit(holder->client,OFF);
  mysql_autocommit(holder->client,OFF);

  int ret = 0;
  if(mysql_query(holder->client,sql1.c_str()))
  {
    errorno = mysql_errno(holder->client);
    errormsg = mysql_error(holder->client);
    BN_LOG_ERROR("error sql:%s",sql1.c_str());
    i = 1;
  }
  if(mysql_query(holder->client,sql2.c_str()))
  {
    BN_LOG_ERROR("error sql:%s",sql2.c_str());
    i = 1;
  }
  if(i == 1)
  {
    mysql_rollback(holder->client);
  }
  else
  {
    mysql_commit(holder->client);		
  }
  mysql_autocommit(holder->client,ON);
  if(1 == i)
  {
    throw new CMy_BSDR_Exception(errormsg.c_str(),errorno+10000);
  }
 
   return 0;
 }

 


 int 
 ndn::CMyMYSQLPL::updateDataAndDeleteFile(CQIHolder * holder,
                  const CReg&creg)
 {
    std::string sql1("");
    sql1.append("update mymysql.");
    sql1.append(__TABLE_DATA);
    sql1.append(creg.chainId);
    sql1.append(" set verification = '");
    sql1.append(creg.verification);
    sql1.append("', version = version+1,content = '");
    sql1.append(creg.content);
    sql1.append("',tid = '");
    sql1.append(creg.tid);
    sql1.append("',price = '");
    sql1.append(creg.price);
    sql1.append("',servfee = '");
    sql1.append(creg.servfee);
    sql1.append("',available = ");
    sql1.append("1");
    sql1.append(",txidstate = 1");
    sql1.append(",stamp = ");
    sql1.append("now()");
    sql1.append(" where blocklabel = '");
    sql1.append(creg.blocklabel);
    sql1.append("' and market = '");
    sql1.append(creg.providerMarket);
    sql1.append("' and devicegroupid = '");
    sql1.append(creg.deviceGroupId);
    sql1.append("';");

    std::string sql2("");
    sql2.append("delete from mymysql.file where blocklabel = '");
    sql2.append(creg.blocklabel);
    sql2.append("';");



    int ON = 1;
  	int OFF = 0;
    int i = 0;
    int errorno = 0;
    std::string errormsg = "";

    pingAliveIfReconn(holder);
  
    mysql_autocommit(holder->client,OFF);
    mysql_autocommit(holder->client,OFF);

    int ret = 0;
    if(mysql_query(holder->client,sql1.c_str()))
    {
      errorno = mysql_errno(holder->client);
      errormsg = mysql_error(holder->client);
      BN_LOG_ERROR("error sql:%s",sql1.c_str());
      i = 1;
    }
    if(mysql_query(holder->client,sql2.c_str()))
    {
      BN_LOG_ERROR("error sql:%s",sql2.c_str());
      i = 1;
    }
    if(i == 1)
    {
      mysql_rollback(holder->client);
    }
    else
    {
      mysql_commit(holder->client);		
    }
    mysql_autocommit(holder->client,ON);
    if(1 == i)
    {
      throw new CMy_BSDR_Exception(errormsg.c_str(),errorno+10000);
    }


   return 0;
 }


 int 
 ndn::CMyMYSQLPL::doDataRegAdd(CQIHolder * holder,
                  const CReg&creg)
  {

  _LOG_INFO("begin doDataRegAdd");
  std::string sql("");
  sql.append("insert into mymysql.");
  sql.append(__TABLE_DATA);
  sql.append(creg.chainId);
  sql.append(" (tid,blocklabel,market,"
             "devicegroupid,verification,"
             "servfee,price,available,"
             "version,stamp,content,txidstate) values ('");
  sql.append(creg.tid);
  sql.append("','");
  sql.append(creg.blocklabel);
  sql.append("','");
  sql.append(creg.providerMarket);
  sql.append("','");
  sql.append(creg.deviceGroupId);
  sql.append("','");
  sql.append(creg.verification);
  sql.append("','");
  sql.append(creg.servfee);
  sql.append("','");
  sql.append(creg.price);
  sql.append("','");
  sql.append("1");
  sql.append("','");
  sql.append("1");
  sql.append("',");
  sql.append("now(),'");
  sql.append(creg.content);
  sql.append("',");
  sql.append("1");
  sql.append(");");

  int ret = 0;
  _PRINT_KEY_VALUE(sql);
  ret = execSQL(holder, sql.c_str());
  if (ret == -1)
  {
   //fprintf(stderr, "addRecordsData error!\n");
    _LOG_ERROR("doDataRegAdd error!\n");
    return -1;
  }

  _LOG_INFO("end doDataRegAdd");
  return 0;

};

  int 
  ndn::CMyMYSQLPL::doDataRegUpdate(CQIHolder * holder,
                  const CReg&creg)
 {
    _LOG_INFO("begin doDataRegUpdate");
    std::string sql("");
    sql.append("update mymysql.");
    sql.append(__TABLE_DATA);
    sql.append(creg.chainId);
    sql.append(" set verification = '");
    sql.append(creg.verification);
    sql.append("', version = version+1,content = '");
    sql.append(creg.content);
    sql.append("',tid = '");
    sql.append(creg.tid);
    sql.append("',price = '");
    sql.append(creg.price);
    sql.append("',servfee = '");
    sql.append(creg.servfee);
    sql.append("',available = ");
    sql.append("1");
    sql.append(",txidstate = 1");
    sql.append(",stamp = ");
    sql.append("now()");
    sql.append(" where blocklabel = '");
    sql.append(creg.blocklabel);
    sql.append("' and market = '");
    sql.append(creg.providerMarket);
    sql.append("' and devicegroupid = '");
    sql.append(creg.deviceGroupId);
    sql.append("';");
    
    int ret = 0;
    ret = execSQL(holder, sql.c_str());
    _PRINT_KEY_VALUE(sql);
    if (ret == -1)
    {
    //fprintf(stderr, "addRecordsData error!\n");
      _LOG_ERROR("doDataRegUpdate error!\n");
      return -1;
    }

    _LOG_INFO("end doDataRegUpdate");
    return 0;
  };
    
  
  int 
  ndn::CMyMYSQLPL::doDataRegBan(CQIHolder * holder,
                  const CReg&creg)
  {
      _LOG_INFO("begin doDataRegBan");
      std::string sql("");
      sql.append("update mymysql.");
      sql.append(__TABLE_DATA);
      sql.append(creg.chainId);
      sql.append(" set available = 0,stamp=now(),version=version+1,txidstate = 1,tid = '");
      sql.append(creg.tid);
      sql.append("' where blocklabel = '");
      sql.append(creg.blocklabel);
      sql.append("';");

      int ret = 0;
      ret = execSQL(holder, sql.c_str());
      if (ret == -1)
      {
        _LOG_ERROR("doDataRegBan error!\n");
        return -1;
      }

    _LOG_INFO("end doDataRegBan");
    return 0;
  };


  int
  ndn::CMyMYSQLPL::regAvailable(CQIHolder *holder,const std::string &blocklabel,const std::string & myTable,int &result)
  {
    MYSQL* client = holder->client;
    MYSQL_RES * cursor;
    MYSQL_FIELD * fieldtitle;
    MYSQL_ROW records;

    _LOG_INFO("begin regAvailable");
    std::string sql("");
    sql.append("select available from");
    sql.append(" mymysql.");
    sql.append(__TABLE_DATA);
    sql.append(myTable);
    sql.append(" where blocklabel = '");
    sql.append(blocklabel);
    sql.append("' and txidstate = 2;");
      
    std::string iStr = std::string("0");
    _PRINT_KEY_VALUE(sql);
     int  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
    if (-1 == ret)
    {
      _LOG_ERROR("pingAliveIfReconn next...");
      return -1;
    }
    ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
     if (ret != 0)
     {
      _LOG_ERROR("sql=" << sql);
      _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
      throwQueryException(client);
     
     }

     cursor = mysql_store_result(client);
    if (cursor)
    {
      while ((fieldtitle = mysql_fetch_field(cursor)))
      {
        _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
      }
      while ((records = mysql_fetch_row(cursor)))
      {
        _LOG_INFO("records=" << records[0]);
        iStr = std::string(records[0]);
      }
      mysql_free_result(cursor);
      while (!mysql_next_result(client))
      {
        cursor = mysql_store_result(client);
        mysql_free_result(cursor);
      }
    }
  result = atoi(iStr.c_str());

  _LOG_INFO("end regAvailable");
  return 0;
  }


  int 
  ndn::CMyMYSQLPL::updateRegPrice(CQIHolder *holder,
                                    std::string &blocklabel,
                                    std::string &price,
                                    std::string &verification,
                                    std::string &mytable)
  {
    _LOG_INFO("begin updateRegPrice");
    std::string sql("");
    sql.append("update mymysql.");
    sql.append(__TABLE_DATA);
    sql.append(mytable);
    sql.append(" set");
    sql.append(" price = '");
    sql.append(price);
    sql.append("', version = version+1,txidstate=2,");
    sql.append("stamp = ");
    sql.append("now()");
    sql.append(" where blocklabel = '");
    sql.append(blocklabel);
    sql.append("';");
    
    int ret = 0;
    ret = execSQL(holder, sql.c_str());
    if (ret == -1)
    {
    //fprintf(stderr, "addRecordsData error!\n");
      _LOG_ERROR("updateRegPrice error!\n");
      return -1;
    }

    _LOG_INFO("end updateRegPrice");
    return 0;
  };



  int 
  ndn::CMyMYSQLPL::queryRegSum(CQIHolder *holder, CReg &reg,int &regcount){
    MYSQL* client = holder->client;
    MYSQL_RES * cursor;
    MYSQL_FIELD * fieldtitle;
    MYSQL_ROW records;

    _LOG_INFO("begin queryRegSum");
    std::string sql("");
    chainTotal = Config::Instance()->portVec.size(); 
    for(int i = 0;i<chainTotal-1;i++)
    {
      sql.append("select blocklabel from mymysql.");
      sql.append(__TABLE_DATA);
      sql.append(std::to_string(i));
      sql.append(" where txidstate = 2 ");
      sql.append(" and market = '");
      sql.append(reg.providerMarket);
      sql.append("' and devicegroupid = '");
      sql.append(reg.deviceGroupId);
      sql.append("' union ");
    }
    sql.append("select blocklabel from");
    sql.append(" mymysql.");
    sql.append(__TABLE_DATA);
    sql.append(std::to_string(chainTotal-1));
    sql.append(" where txidstate = 2 ");
    sql.append(" and market = '");
    sql.append(reg.providerMarket);
    sql.append("' and devicegroupid = '");
    sql.append(reg.deviceGroupId);
    sql.append("';");
      
    _PRINT_KEY_VALUE(sql);
    int  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
    if (-1 == ret)
    {
      _LOG_ERROR("pingAliveIfReconn next...");
      return -1;
    }
     ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
     if (ret != 0)
     {
      _LOG_ERROR("sql=" << sql);
      _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
      throwQueryException(client);
     
     }

     cursor = mysql_store_result(client);
    if (cursor)
    {
      while ((fieldtitle = mysql_fetch_field(cursor)))
      {
        _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
      }
      while ((records = mysql_fetch_row(cursor)))
      {
         regcount++;
      }
      mysql_free_result(cursor);
      while (!mysql_next_result(client))
      {
        cursor = mysql_store_result(client);
        mysql_free_result(cursor);
      }
    }
   
  _LOG_INFO("end queryRegSum");
    return 0;
  }

  int 
  ndn::CMyMYSQLPL::queryRegPrice(CQIHolder *holder,CReg &reg,std::string &txid){
    MYSQL* client = holder->client;
    MYSQL_RES * cursor;
    MYSQL_FIELD * fieldtitle;
    MYSQL_ROW records;

    _LOG_INFO("begin queryRegPrice");
    std::string sql("");
    sql.append("select price,txid from");
    sql.append(" mymysql.");
    sql.append(__TABLE_DATA);
    sql.append(reg.chainId);
    sql.append(" where txidstate = 2 and blocklabel = '");
    sql.append(reg.blocklabel);
    sql.append("' and market = '");
    sql.append(reg.providerMarket);
    sql.append("' and devicegroupid = '");
    sql.append(reg.deviceGroupId);
    sql.append("' ;");
      
    _PRINT_KEY_VALUE(sql);
    int ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
     if (-1 == ret)
     {
        _LOG_ERROR("pingAliveIfReconn next...");
     }
     ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
     if (ret != 0)
     {
      _LOG_ERROR("sql=" << sql);
      _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
      throwQueryException(client);

     }

     cursor = mysql_store_result(client);
    if (cursor)
    {
      while ((fieldtitle = mysql_fetch_field(cursor)))
      {
        _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
      }
      while ((records = mysql_fetch_row(cursor)))
      {
        _LOG_INFO("records=" << records[0]);
        reg.price = std::string(records[0]);
        txid = std::string(records[1]);
      }
      mysql_free_result(cursor);
      while (!mysql_next_result(client))
      {
        cursor = mysql_store_result(client);
        mysql_free_result(cursor);
      }
    }
    

  _LOG_INFO("end queryRegPrice");
    return 0;
  }

int ndn::CMyMYSQLPL::queryRegRecords(CQIHolder * holder,ndn::CReg creg,ndn::RegResult&rreg){
 
  MYSQL* client = holder->client;
  MYSQL_RES * cursor;
  MYSQL_FIELD * fieldtitle;
  MYSQL_ROW records;

  std::string sql = std::string("select t.txid,t.price, t.content from mymysql.data");
  sql.append(creg.chainId);
  sql.append(" t where t.blocklabel = ");
  sql.append("'");
  sql.append(creg.blocklabel);
  sql.append("'");

  _PRINT_KEY_VALUE(sql);
  int  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
  if (-1 == ret)
  {
    _LOG_ERROR("pingAliveIfReconn next...");
    return -1;
  }
  
    ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
    if (ret != 0)
    {
    _LOG_ERROR("sql=" << sql);
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
    throwQueryException(client);
    }

    cursor = mysql_store_result(client);
  if (cursor)
  {
    while ((fieldtitle = mysql_fetch_field(cursor)))
    {
      _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
    }
    while ((records = mysql_fetch_row(cursor)))
    {
      _LOG_INFO("records=" << records[0]);
      rreg.txid = std::string(records[0]);
      rreg.price = std::string(records[1]);
      rreg.content = std::string(records[2]);
      
      
    }
    mysql_free_result(cursor);
    while (!mysql_next_result(client))
    {
      cursor = mysql_store_result(client);
      mysql_free_result(cursor);
    }
  }
  
  return 0;
}



int 
ndn::CMyMYSQLPL::insertS3File(CQIHolder *holder,S3FileInfo &output){

_LOG_INFO("select insertS3File");
std::string sql("");
sql.append("insert into file ( blocklabel,s3host,s3access,s3secret,s3bucket,s3srcfilename,s3srcfilesize,s3dstfile,s3user ) values ('");
sql.append(output.blocklabel);
sql.append("','");
sql.append(output.s3Host);
sql.append("','");
sql.append(output.s3Access);
sql.append("','");
sql.append(output.s3Secret);
sql.append("','");
sql.append(output.s3Bucket);
sql.append("','");
sql.append(output.s3SrcFileName);
sql.append("','");
sql.append(std::to_string(output.s3SrcFileSize));
sql.append("','");
sql.append(output.s3DstFile);
sql.append("','");
sql.append(output.s3User);
sql.append("');");

int ret = 0;
ret = execSQL(holder, sql.c_str());
if (ret == -1)
{
  //fprintf(stderr, "addRecordsData error!\n");
  _LOG_ERROR("insertS3File error!\n");
  return -1;
}

_LOG_INFO("end insertS3File");
return 0;
}

int 
ndn::CMyMYSQLPL::updateS3File(CQIHolder *holder,S3FileInfo &input){


  MYSQL* client = holder->client;
  MYSQL_RES * cursor;
  MYSQL_FIELD * fieldtitle;
  MYSQL_ROW records;

  std::string sql = std::string("update mymysql.file");
  sql.append(" set s3host = '");
  sql.append(input.s3Host);
  sql.append("',s3access = '");
  sql.append(input.s3Access);
  sql.append("',s3Bucket = '");
  sql.append(input.s3Bucket);
  sql.append("',s3DstFile = '");
  sql.append(input.s3DstFile);
  sql.append("',s3Secret = '");
  sql.append(input.s3Secret);
  sql.append("',s3SrcFileName = '");
  sql.append(input.s3SrcFileName);
  sql.append("',digestf = '");
  sql.append(input.digestf);
  sql.append("',s3SrcFileSize = '");
  sql.append(std::to_string(input.s3SrcFileSize));
  sql.append("',s3User = '");
  sql.append(input.s3User);
  sql.append("' where blocklabel = '");
  sql.append(input.blocklabel);
  sql.append("';");
  
      

  _PRINT_KEY_VALUE(sql);
   int  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
   execSQL(holder,sql.c_str());

  return 0;
}


int 
ndn::CMyMYSQLPL::selectS3File(CQIHolder *holder,S3FileInfo &output){


  MYSQL* client = holder->client;
  MYSQL_RES * cursor;
  MYSQL_FIELD * fieldtitle;
  MYSQL_ROW records;

  std::string sql = std::string("select t.blocklabel,t.s3host,t.s3access,t.s3secret,t.s3bucket,t.s3srcfilename,t.s3srcfilesize,t.s3dstfile,t.s3user,t.digestf from mymysql.file t where t.blocklabel = ");
  sql.append("'");
  sql.append(output.blocklabel);
  sql.append("';");
  
      

  _PRINT_KEY_VALUE(sql);
   int  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);

   ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
  if (ret != 0)
  {
    _LOG_ERROR("sql=" << sql);
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
     throwQueryException(client);
  }

  cursor = mysql_store_result(client);
  if (cursor)
  {
    while ((fieldtitle = mysql_fetch_field(cursor)))
    {
      _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
    }
    while ((records = mysql_fetch_row(cursor)))
    {
      _LOG_INFO("records=" << records[0]);//这里的东西都不允许为NULL
      output.blocklabel = std::string(records[0]);
      output.s3Host = std::string(records[1]);
      output.s3Access = std::string(records[2]);
      output.s3Secret = std::string(records[3]);
      output.s3Bucket = std::string(records[4]);
      if(NULL == records[5])
      {
        output.s3SrcFileName = std::string("");
      }else{
        output.s3SrcFileName = std::string(records[5]);
      }
      output.s3SrcFileSize = atol(records[6]);
      output.s3DstFile = std::string(records[7]);
      output.s3User = std::string(records[8]);
      output.digestf = std::string(records[9]);

    }
    mysql_free_result(cursor);
    while (!mysql_next_result(client))
    {
      cursor = mysql_store_result(client);
      mysql_free_result(cursor);
    }
  }

  return 0;
}

int 
ndn::CMyMYSQLPL::queryConsumeSum(CQIHolder *holder, CPay &cpay){
    MYSQL* client = holder->client;
    MYSQL_RES * cursor;
    MYSQL_FIELD * fieldtitle;
    MYSQL_ROW records;

    _LOG_INFO("begin queryConsumeSum");
    std::string sql("");
    sql.append("select count(amount) from");
    sql.append(" mymysql.");
    sql.append(__TABLE_TX);
    sql.append(cpay.chainId);
    sql.append(" where blocklabel = '");
    sql.append(cpay.blocklabel);
    sql.append("' and market = '");
    sql.append(cpay.providerMarket);
    sql.append("' and devicegroupid = '");
    sql.append(cpay.deviceGroupId);
    sql.append("' group by amount;");
      
    
    _PRINT_KEY_VALUE(sql);
     int  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
     if (-1 == ret)
     {
      _LOG_ERROR("pingAliveIfReconn next...");
      return -1;
     }
      ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
     if (ret != 0)
     {
      _LOG_ERROR("sql=" << sql);
      _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
      throwQueryException(client);
     }

     cursor = mysql_store_result(client);
    if (cursor)
    {
      while ((fieldtitle = mysql_fetch_field(cursor)))
      {
        _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
      }
      while ((records = mysql_fetch_row(cursor)))
      {
        _LOG_INFO("records=" << records[0]);
        cpay.payMount  = std::string(records[0]);
      }
      mysql_free_result(cursor);
      while (!mysql_next_result(client))
      {
        cursor = mysql_store_result(client);
        mysql_free_result(cursor);
      }
    }

  _LOG_INFO("end queryRegSum");
    return 0;
};

int ndn::CMyMYSQLPL::queryConsumeBill(CQIHolder *holder,CPay &cpay)
{

  MYSQL* client = holder->client;
  MYSQL_RES * cursor;
  MYSQL_FIELD * fieldtitle;
  MYSQL_ROW records;

  std::string sql("");
  sql.append("select sum(amount) from mymysql.tx");
  sql.append(cpay.chainId);
  sql.append(" where blocklabel = '");
  sql.append(cpay.blocklabel);
  sql.append("' and market = '");
  sql.append(cpay.providerMarket);
  sql.append("' and devicegroupid = '");
  sql.append(cpay.deviceGroupId);
  sql.append("' group by blocklabel;");

  int  ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
  if (-1 == ret)
  {
    _LOG_ERROR("pingAliveIfReconn next...");
    return -1;
  }

  ret =  mysql_real_query(client,sql.c_str(),strlen(sql.c_str()));
  if(-1 == ret)
  {
    _LOG_ERROR("sql = "<<sql);
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
     throwQueryException(client);
  }

  cursor = mysql_store_result(client);
  if (cursor)
  {
    while ((fieldtitle = mysql_fetch_field(cursor)))
    {
      _LOG_INFO("fieldtitle->name=" << fieldtitle->name);
    }
    while ((records = mysql_fetch_row(cursor)))
    {
      _LOG_INFO("records=" << records[0]);
      cpay.payMount = std::string(records[0]);
    }
    mysql_free_result(cursor);
    while (!mysql_next_result(client))
    {
      cursor = mysql_store_result(client);
      mysql_free_result(cursor);
    }
  }
    

    _LOG_INFO("end queryConsumeBill");;
    
  return 0;
}

