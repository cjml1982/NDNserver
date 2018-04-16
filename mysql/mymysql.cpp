
#include "mymysql.hpp"
//begin 20170629 liulijin add
#include "../logging4.h"
#include "../openssl/myopenssl.hpp"
#include "../constant/myconstant.hpp"
#include "mysqlerrcode.hpp"
//end 20170629 liulijin add

INIT_LOGGER("CMyMYSQL");

ndn::CMyMYSQL * ndn::CMyMYSQL::instance = new CMyMYSQL;

ndn::CMyMYSQL *
ndn::CMyMYSQL::getInstance()
{
  return instance;
}

int
ndn::CMyMYSQL::pingAlive()
{
  _LOG_INFO("begin pingAlive");

  if (mysql_ping(client))
  {
    //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_INFO("Error:" << mysql_errno(client) << "," << mysql_error(client));
    switch ((MYSQL_ERR_CODE)mysql_errno(client))
    {
      case MYSQL_ERR_CODE::MY_MYSQL_UNKNOWN_ERROR:
        throw new CMyMySQLResultException{"ping err", __GLOBAL_MY_MYSQL_UNKNOWN_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SERVER_GONE_ERROR:
        throw new CMyMySQLResultException{"ping err", __GLOBAL_MY_MYSQL_SERVER_GONE_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_COMMANDS_OUT_OF_SYNC:
        throw new CMyMySQLResultException{"ping err", __GLOBAL_MY_MYSQL_COMMANDS_OUT_OF_SYNC};
        break;
      default: break;
    }

    return -1;
  }

  _LOG_INFO("end pingAlive");
  return 0;
}

int
ndn::CMyMYSQL::pingAliveIfReconn()
{
  _LOG_INFO("begin pingAliveIfReconn");

  int ret = 0;
  ret = pingAlive();
  if (ret == -1)
  {
    ret = closeMYSQL();
    if (ret == -1)
    {
      //fprintf(stderr, "closeMYSQL error!\n");
      _LOG_INFO("closeMYSQL error!");
      return -1;
    }
   
    for(int i = 0;i<(Config::Instance()->dbvec.size());i++)
    {
      
      ret = connectMYSQL(((Config::Instance()->dbvec[i]).ip).c_str(), __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
      if(ret!=-1)
      {
            break;
      }
    }
    if (ret == -1)
    {
      //fprintf(stderr, "connectMYSQL error!\n");
      _LOG_INFO("connectMYSQL error!");
      return -1;
    }
  }

  _LOG_INFO("end pingAliveIfReconn");
  return 0;
}


MYSQL *
ndn::CMyMYSQL::getClient()
{
  return client;
}

int
ndn::CMyMYSQL::execSQL(const char * sql)
{
  _LOG_INFO("begin execSQL");

  int ret = 0;
  ret = pingAliveIfReconn();
  if (ret == -1)
  {
    //fprintf(stderr, "pingAliveIfReconn error!\n");
    _LOG_INFO("pingAliveIfReconn error!");
    return -1;
  }

 

  if (mysql_query(client, sql))
  {
   //fprintf(stderr, "%s\n", sql);
    _LOG_INFO("sql=" << sql);
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_INFO("Error:" << mysql_errno(client) << "," << mysql_error(client));
    if(1062 == mysql_errno(client))
        throw new CMy_BSDR_Exception("__GLOBAL_DB_EXIST_ITEM_ERR",__GLOBAL_DB_EXIST_ITEM_ERR);
    switch ((MYSQL_ERR_CODE)mysql_errno(client))
    {
      case MYSQL_ERR_CODE::MY_MYSQL_UNKNOWN_ERROR:
        throw new CMyMySQLResultException{"query err", __GLOBAL_MY_MYSQL_UNKNOWN_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SERVER_GONE_ERROR:
        throw new CMyMySQLResultException{"query err", __GLOBAL_MY_MYSQL_SERVER_GONE_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_COMMANDS_OUT_OF_SYNC:
        throw new CMyMySQLResultException{"query err", __GLOBAL_MY_MYSQL_COMMANDS_OUT_OF_SYNC};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SERVER_LOST:
        throw new CMyMySQLResultException{"query err", __GLOBAL_MY_MYSQL_SERVER_LOST};
        break;
      
      default: break;
    }
    return -1;
  }

  _LOG_INFO("end execSQL");
  return 0;
}

int
ndn::CMyMYSQL::getMYSQLClientVersion()
{
  _LOG_INFO("begin getMYSQLClientVersion");

 //fprintf(stderr, "MySQL client version: %s\n", mysql_get_client_info());
  _LOG_INFO("MySQL client version: " << mysql_get_client_info());

  _LOG_INFO("end getMYSQLClientVersion");
  return 0;
}

int
ndn::CMyMYSQL::initMYSQL()
{
  _LOG_INFO("begin initMYSQL");

  if (client == NULL)
  {
    client = mysql_init(NULL);
  }

  if (client == NULL)
  {
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
      throw new CMy_BSDR_Exception("__GLOBAL_MY_MYSQL_CONNECTION_ERROR",__GLOBAL_MY_MYSQL_CONNECTION_ERROR);
  }

  _LOG_INFO("end initMYSQL");
  return 0;
}

int
ndn::CMyMYSQL::connectMYSQL(const char * ip,
                            const char * usr,
                            const char * pswd,
                            const char * db)
{
  _LOG_INFO("begin connectMYSQL");

  unsigned int connTimeOut = __MYSQL_CONN_TIMEOUT_MILS;
  int ret = mysql_options(client, MYSQL_OPT_CONNECT_TIMEOUT, &connTimeOut);
  if (ret != 0)
  {
    _LOG_INFO("could not set SQL options");
    mysql_close(client);
    client = NULL;
    return -1;
  }

  unsigned int port=0;
  const char * unixSock = 0;
  unsigned long flag=0;
  client = mysql_real_connect(client, ip, usr, pswd, db, port, unixSock, flag);
  if (client == NULL)
  {
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_INFO("Error:" << mysql_errno(client) << "," << mysql_error(client));
    switch ((MYSQL_ERR_CODE)mysql_errno(client))
    {
      case MYSQL_ERR_CODE::MY_MYSQL_CONN_HOST_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_CONN_HOST_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_CONNECTION_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_CONNECTION_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_IPSOCK_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_IPSOCK_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_OUT_OF_MEMORY:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_OUT_OF_MEMORY};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SOCKET_CREATE_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_SOCKET_CREATE_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_UNKNOWN_HOST:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_UNKNOWN_HOST};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_VERSION_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_VERSION_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_NAMEDPIPEOPEN_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_NAMEDPIPEOPEN_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_NAMEDPIPEWAIT_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_NAMEDPIPEWAIT_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_NAMEDPIPESETSTATE_ERROR:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_NAMEDPIPESETSTATE_ERROR};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_SERVER_LOST:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_SERVER_LOST};
        break;
      case MYSQL_ERR_CODE::MY_MYSQL_ALREADY_CONNECTED:
        throw new CMyMySQLResultException{"connect err", __GLOBAL_MY_MYSQL_ALREADY_CONNECTED};
        break;
      default: break;
    }
    return -1;
  }

  ret = mysql_set_character_set(client, "utf8mb4");
  if (ret != 0)
  {
   //fprintf(stderr, "Error %u: %s\n", mysql_errno(client), mysql_error(client));
    _LOG_INFO("Error:" << mysql_errno(client) << "," << mysql_error(client));
    mysql_close(client);
    client = NULL;
    //no throw code could be use.
    return -1;
  }

  _LOG_INFO("end connectMYSQL");
  return 0;
}

int
ndn::CMyMYSQL::closeMYSQL()
{
  _LOG_INFO("begin closeMYSQL");

  if (NULL != client)
  {
    mysql_close(client);
  }
  //note
  client = NULL;

  _LOG_INFO("end closeMYSQL");
  return 0;
}

int
ndn::CMyMYSQL::hasTable(const std::string & wildcard)
{
  _LOG_INFO("begin hasTable");

  MYSQL_RES * cursor;
  MYSQL_ROW records;

  cursor = mysql_list_tables(client, wildcard.c_str());
  if (cursor)
  {
    records = mysql_fetch_row(cursor);
    if (!records)
    {
     //fprintf(stderr, "no table matched!\n");
      _LOG_INFO("no table matched!\n");
      return -1;
    }

    mysql_free_result(cursor);
    while (!mysql_next_result(client))
    {
      cursor = mysql_store_result(client);
      mysql_free_result(cursor);
    }
  }

  cursor = mysql_list_tables(client, wildcard.c_str());
  if (cursor)
  {
    while ((records = mysql_fetch_row(cursor)))
    {
     //fprintf(stderr, "%s \n", records[0]);
      _LOG_INFO("records: " << records[0]);
    }

    mysql_free_result(cursor);
    while (!mysql_next_result(client))
    {
      cursor = mysql_store_result(client);
      mysql_free_result(cursor);
    }
  }

  _LOG_INFO("end hasTable");
  return 0;
}

int
ndn::CMyMYSQL::createDB(const char * sql)
{
  _LOG_INFO("begin createDB");

  int ret = 0;
  ret = execSQL(sql);
  if (ret == -1)
  {
   //fprintf(stderr, "createDB error!\n");
    _LOG_INFO("createDB error!\n");
    return -1;
  }

  _LOG_INFO("end createDB");
  return 0;
}



int
ndn::CMyMYSQL::createTable(const char * sql)
{
  _LOG_INFO("begin createTable");

  int ret = 0;
  ret = execSQL(sql);
  if (ret == -1)
  {
   //fprintf(stderr, "createTable error!\n");
    _LOG_INFO("createTable error!\n");
    return -1;
  }

  _LOG_INFO("end createTable");
  return 0;
}



//end michael added
