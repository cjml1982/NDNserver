
#ifndef MYBIZ_HPP
#define MYBIZ_HPP
#include <vector>
#include <unistd.h>
#include "curl/mycurl.hpp"
#include "json/myjson.hpp"
#include "tinyxml/myxml.hpp"
#include "mysql/mymysql.hpp"
#include "openssl/myopenssl.hpp"
#include "constant/myconstant.hpp"
#include "tinyxml/tinyxml.h"
//begin 20170601 liulijin add
#include "logging4.h"

#include "config.h"


#ifdef __cplusplus
extern "C" {
#endif

namespace ndn {


class CMyBIZ {

public:

  CMyBIZ()
  {

  }
  ~CMyBIZ() {}

  static CMyBIZ *
  getInstance();

public:

  int
  initBIZLocal();


  int 
  getDBServerWithNoDB();

  int 
  getDBServerWithDB();

  int
  closeBIZLocal();


public:

  int
  initMYSQL();

  int
  closeMYSQL();

  int
  createMySQLDB();

  int
  createMySQLDataTable();

  int
  createMySQLTXTable();

  
  int 
  createMarketTable();

  int
  createFileTable();

  int
  initMainTLib();






public:

 
  int
  getAddressesByAccountinitWalletHashMap(std::string & sURL,

                                     std::string & sHeader,
                                     std::string & address);

  

private:
  static CMyBIZ * instance;

};

}

#ifdef __cplusplus
}
#endif

#endif
