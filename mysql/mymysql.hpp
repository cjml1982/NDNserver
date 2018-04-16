
#ifndef MYMYSQL_HPP
#define MYMYSQL_HPP

#include <my_global.h>
#include <mysql.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>
#include "../config.h"
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMyMySQLResultException : public CMy_BSDR_Exception
{
public:
  CMyMySQLResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CMyMySQLResultException() {};
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

namespace ndn {

class CMyMYSQL {

public:

  CMyMYSQL()
  {
    client = NULL;
  }
  ~CMyMYSQL() {}

  static CMyMYSQL *
  getInstance();

public:

  int
  pingAlive();

  int
  pingAliveIfReconn();

public:

  MYSQL *
  getClient();

public:

  int
  execSQL(const char * sql);

  int
  getMYSQLClientVersion();

public:

  int
  initMYSQL();

  int
  connectMYSQL(const char * ip,
               const char * usr,
               const char * pswd,
               const char * db);

  int
  closeMYSQL();

public:

  int
  hasTable(const std::string & wildcard);

  int
  createDB(const char * sql);
  
  int
  createTable(const char * sql);


private:
  static CMyMYSQL * instance;

  MYSQL * client;
};

}

#ifdef __cplusplus
}
#endif

#endif

