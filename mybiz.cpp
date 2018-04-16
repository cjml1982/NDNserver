
#include "mybiz.hpp"


//end 20170601 liulijin add

INIT_LOGGER("CMyBIZ");

ndn::CMyBIZ * ndn::CMyBIZ::instance = new CMyBIZ;


ndn::CMyBIZ *
ndn::CMyBIZ::getInstance()
{
  return instance;
}

int
ndn::CMyBIZ::getDBServerWithNoDB()
{
    int i = 0;
    for(;i<Config::Instance()->dbvec.size();i++)
    {
      
      int ret = ndn::CMyMYSQL::getInstance()->connectMYSQL(((Config::Instance()->dbvec[i]).ip).c_str(), __MYSQL_USER, __MYSQL_PASSWORD, NULL);
      if (ret != -1)
      {
   
        BN_LOG_INTERFACE("initBIZLocal connectMYSQL good!");
        break;
      }
    }
    if(i==(Config::Instance()->dbvec).size())
    {
      BN_LOG_ERROR("nodbConnet");
      //throw new CMy_BSDR_Exception("__GLOBAL_MY_MYSQL_UNKNOWN_ERRO",__GLOBAL_MY_MYSQL_UNKNOWN_ERROR);
    }
    return 0;
}

int
ndn::CMyBIZ::getDBServerWithDB()
{
    int i = 0;
    for(;i<Config::Instance()->dbvec.size();i++)
    {
      
      int ret = ndn::CMyMYSQL::getInstance()->connectMYSQL(((Config::Instance()->dbvec[i]).ip).c_str(), __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
      if (ret != -1)
      {
    
        BN_LOG_INTERFACE("initBIZLocal connectMYSQL good!");
        break;
      }
    }
    if(i==(Config::Instance()->dbvec).size())
    {
       _LOG_ERROR("mysql connect error!");
      throw new CMy_BSDR_Exception("__GLOBAL_MY_MYSQL_CONNECTION_ERROR",__GLOBAL_MY_MYSQL_CONNECTION_ERROR);
    }
    return 0;
}

int
ndn::CMyBIZ::initBIZLocal()
{
  BN_LOG_INTERFACE("begin initBIZLocal");

    ndn::CMyMYSQL::getInstance()->initMYSQL();
   BN_LOG_INTERFACE("end initBIZLocal");

  return getDBServerWithDB();

}

int
ndn::CMyBIZ::closeBIZLocal()
{
  BN_LOG_INTERFACE("begin closeBIZLocal");

  ndn::CMyMYSQL::getInstance()->closeMYSQL();
  //ndn::CMyCURL::getInstance()->curlDestroy();

  BN_LOG_INTERFACE("end closeBIZLocal");
  return 0;
}

int 
ndn::CMyBIZ::createFileTable()
{ 
  
  BN_LOG_INTERFACE("begin createFileTable");
  //for循环 建链
  int ret = 0;
  ndn::CMyMYSQL::getInstance()->initMYSQL();

  //ret = ndn::CMyMYSQL::getInstance()->connectMYSQL(__MYSQL_IP, __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
  getDBServerWithDB();

  int len = Config::Instance()->portVec.size();
  std::string sql = std::string("create table if not exists mymysql.file");
  char * FILE_TABLE = " (id int(255) primary key not null auto_increment,"\
                          "blocklabel varchar(255) not null unique," \
                          "s3host varchar(255) not null," \
                          "s3access varchar(255) not null," \
                          "s3secret varchar(255) not null," \
                          "s3bucket varchar(255) not null," \
                          "s3srcfilename varchar(255)," \
                          "s3srcfilesize int(255)," \
                          "s3dstfile varchar(255) not null,"
                          "s3user varchar(255) not null," \
                          "digestf varchar(255) default ''," \
                          "uploadstatus tinyint(1) default 0" \
                          ") engine=InnoDB default charset=utf8mb4;";

  sql.append(std::string(FILE_TABLE));
  //end michael added
  BN_LOG_INTERFACE("sql:%s\n",sql.c_str());
  
  ndn::CMyMYSQL::getInstance()->createTable(sql.c_str());

  ndn::CMyMYSQL::getInstance()->closeMYSQL();
  //begin michael added
  BN_LOG_INTERFACE("end createFileTable");
  return 0;
}
int
ndn::CMyBIZ::createMySQLDB()
{
  BN_LOG_INTERFACE("begin createMySQLDB");

  //std::string createDBSQL("create database if not exists ");
  ndn::CMyMYSQL::getInstance()->initMYSQL();
  //ndn::CMyMYSQL::getInstance()->connectMYSQL(__MYSQL_IP, __MYSQL_USER, __MYSQL_PASSWORD, NULL);
  getDBServerWithNoDB();

  std::string createDBSQL("create database ");
  createDBSQL.append(__MYSQL_DATABASE);
  createDBSQL.append(" default character set utf8mb4 collate utf8mb4_unicode_ci");
  createDBSQL.append(";");

   ndn::CMyMYSQL::getInstance()->createDB(createDBSQL.c_str());

   ndn::CMyMYSQL::getInstance()->closeMYSQL();
  BN_LOG_INTERFACE("end createMySQLDB");

  return 0;
}

int
ndn::CMyBIZ::createMySQLDataTable()
{
  BN_LOG_INTERFACE("begin createMySQLDataTable");
  //for循环 建链
   int ret = 0;
   ndn::CMyMYSQL::getInstance()->initMYSQL();
  //ret = ndn::CMyMYSQL::getInstance()->connectMYSQL(__MYSQL_IP, __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
   getDBServerWithDB();

  int len = Config::Instance()->portVec.size();
  for(int i = 0;i<len;i++)
  {
            std::string sql = std::string("create table if not exists mymysql.data");
            sql.append(std::to_string(i));
            
            char * DATA_TABLE = " ( id int(255) primary key not null auto_increment,"\
                                    "blocklabel varchar(255) not null unique," \
                                    "tid varchar(255) not null," \
                                    "market varchar(255) not null," \
                                    "devicegroupid varchar(255) not null," \
                                    "price varchar(255) not null DEFAULT ''," \
                                    "servfee varchar(255) DEFAULT ''," \
                                    "content varchar(5000) default ''," \
                                    "verification varchar(255) not null ," \
                                    "txid varchar(255) DEFAULT ''," \
                                    "available tinyint(1) DEFAULT 0," \
                                    "version int(10) DEFAULT 0," \
                                    "txidstate tinyint(1) DEFAULT 0,"\
                                    "stamp datetime NOT NULL DEFAULT NOW()" \
                                    ") engine=InnoDB default charset=utf8mb4;";

            sql.append(std::string(DATA_TABLE));
            //end michael added
            BN_LOG_INTERFACE("sql:%s",sql.c_str());
            
            int ret = ndn::CMyMYSQL::getInstance()->createTable(sql.c_str());
  }
  ndn::CMyMYSQL::getInstance()->closeMYSQL();
  //begin michael added
  BN_LOG_INTERFACE("end createMySQLDataTable");
  return 0;
}

int
ndn::CMyBIZ::createMySQLTXTable()
{
  BN_LOG_INTERFACE("begin createMySQLTXTable");
  int len = Config::Instance()->portVec.size();
  ndn::CMyMYSQL::getInstance()->initMYSQL();

  getDBServerWithDB();
 
  for(int i = 0;i<len;i++)
  {
        std::string sql = std::string("create table if not exists mymysql.tx");
        sql.append(std::to_string(i));
        char * TX_TABLE = "( id int(255) not null auto_increment," \
                          "blocklabel varchar(255) not null," \
                          "market varchar(255) not null," \
                          "devicegroupid varchar(255) not null," \
                          "servfee varchar(255) DEFAULT ''," \
                          "txid varchar(255) not null," \
                          "amount varchar(255) not null," \
                          "stamp datetime NOT NULL DEFAULT NOW()," \
                          "primary key (id)"\
                          ") engine=InnoDB default charset=utf8mb4;";

    int ret = 0;
    sql.append(std::string(TX_TABLE));
    ret = ndn::CMyMYSQL::getInstance()->createTable(sql.c_str());

  }
  ndn::CMyMYSQL::getInstance()->closeMYSQL();
  BN_LOG_INTERFACE("end createMySQLTXTable");
  return 0;
}


int
ndn::CMyBIZ::initMainTLib()
{
  BN_LOG_INTERFACE("begin initMainTLib");

  int ret = 0;
  //int argc = 0;
  //char ** argv = NULL;
  //char ** groups = NULL;
  ret = mysql_library_init(0, NULL, NULL);
  if (ret != 0)
  {
   //fprintf(stderr, "could not initialize SQL library at MainT!\n");
    BN_LOG_ERROR("could not initialize SQL library at MainT!");
    mysql_library_end();
    return -1;
  }

  BN_LOG_INTERFACE("end initMainTLib");
  return 0;
}


int
ndn::CMyBIZ::getAddressesByAccountinitWalletHashMap(std::string & sURL,
                                                std::string & sHeader,
                                                std::string & address)
{
 // BN_LOG_INTERFACE("begin getAddressesByAccountinitWalletHashMap");

  std::string methodValue(__CONSENSUS_RPC_METHOD_GETADDRESSESBYACCOUNT);
  std::string paramsValue("");
  std::string bodyJSON("");
  int responseCode = -1;
  std::string response("");
  ndn::CMyJSON::getInstance()->writeJSON(methodValue, paramsValue, bodyJSON);
  BN_LOG_INTERFACE("addrJSON:%s " , bodyJSON.c_str());
  ndn::CMyCURL::getInstance()->doHTTPPost(sURL, sHeader, bodyJSON, responseCode, response);
  BN_LOG_INTERFACE("responseCode:%d " , responseCode);
  BN_LOG_INTERFACE("response:%s " , response.c_str());

  int times = 1;
  int clocks[3] = {15, 30, 60};
  while ( (times < 4) && (responseCode != 200) )
  {
    BN_LOG_INTERFACE("Now, do 3 times trying ... %d" , times);
    sleep(clocks[times - 1]);
    times++;
    ndn::CMyCURL::getInstance()->doHTTPPost(sURL, sHeader, bodyJSON, responseCode, response);
    BN_LOG_INTERFACE("responseCode: %d" , responseCode);
    BN_LOG_INTERFACE("response: %s" ,response.c_str());
  }

  if (responseCode != 200)
  {
      BN_LOG_ERROR("curl post request error!");
      throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
  }

  std::string jsResult("");
  std::string jsError("");
  std::string jsId("");
  ndn::CMyJSON::getInstance()->readJSON(response, jsResult, jsError, jsId);
  BN_LOG_INTERFACE("jsResult: %s",jsResult.c_str());
  BN_LOG_INTERFACE("jsError: %s" , jsError.c_str());
  BN_LOG_INTERFACE("jsId: %s" , jsId.c_str());

  if (jsResult.empty())
  {
    BN_LOG_ERROR("jsResult is empty!");
    return -1;
  }

  address = jsResult;
 // BN_LOG_INTERFACE("end getAddressesByAccountinitWalletHashMap");
  return 0;
}



//begin liangyuanting add
int 
ndn::CMyBIZ::createMarketTable()
{
   BN_LOG_INTERFACE("begin createMarketTable");
   ndn::CMyMYSQL::getInstance()->initMYSQL();
  // ndn::CMyMYSQL::getInstance()->connectMYSQL(__MYSQL_IP, __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
  
   char * MK_TABLE = "create table if not exists mymysql.market"\
                    " (id int(255)  primary key not null auto_increment," \
                    "devicegroupid varchar(255) not null ," \
                    "market varchar(255) unique  not null," \
                    "balance varchar(255) not null" \
                    ") engine=InnoDB default charset=utf8mb4;";

    int ret = 0;
    ret = getDBServerWithDB();
    ret = ndn::CMyMYSQL::getInstance()->createTable(MK_TABLE);

    ndn::CMyMYSQL::getInstance()->closeMYSQL();
    BN_LOG_INTERFACE("end createMarketTable");
  
  return 0;
}

//end liangyuanting end
//
