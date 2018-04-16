
#ifndef MYXML_HPP
#define MYXML_HPP
#include "tinyxml.h"
//begin 20170601 liulijin add
#include "../logging4.h"
#include "../openssl/myopenssl.hpp"
#include "../constant/myconstant.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <vector>
using namespace std;


//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMyXMLResultException : public CMy_BSDR_Exception
{
public:
  CMyXMLResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CMyXMLResultException() {};
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

class CMyXML {

public:

  CMyXML() {}
  ~CMyXML() {}

  static CMyXML *
  getInstance();

public:

  int
  loadLocalName(const std::string & xmlfile, std::string & result);

  int
  createDataContentError(const std::string & code,
                         const std::string & reason,
                         std::string & result);

  int
  createDataContentContentListItem(const std::string & blockedlabel,
                                   const std::string & marketer,
                                   const std::string & price,
                                   const std::string & srcdeviceno,
                                   const std::string & timestamp,
                                   const std::string & content,
                                   std::string & item);

  int
  createDataContentContentList(std::vector<std::string> & items,
                               std::string & result);

  int
  createDataContentPurchaseListItem(const std::string & blockedlabel,
                                    const std::string & marketer,
                                    const std::string & txid,
                                    const std::string & txcontent,
                                    std::string & item);

  int
  createDataContentPurchaseList(std::vector<std::string> & items,
                                std::string & result);

//begin 20170824 liangyuanting
  int 
  createCountXML(const std::string &count,std::string &item);

  int
  createOneXML(const std::string &item,std::string &result);
//end 20170824 liangyuanting
  int
  parseXMLtoArray(const string & input,
                  std::vector<std::string> & items);

  int
  regMoreInfoCreate(std::vector<std::string> & input,
                    std::string & output);

  int
  payMoreInfoCreate(std::vector<std::string> & input,
                    std::string & output);

  int
  regMoreInfoParse(const string & input);

  int
  payMoreInfoParse(const string & input);

private:
  static CMyXML * instance;
};

}

#endif
