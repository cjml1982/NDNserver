
#ifndef MYCURLPL_HPP
#define MYCURLPL_HPP

#include <string.h>

#include <curl/curl.h>

#include <string>
#include <iostream>
#include <mutex>

#include "../myholder.hpp"

using namespace std;

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMyCurlplResultException : public CMy_BSDR_Exception
{
public:
  CMyCurlplResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CMyCurlplResultException() {};
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

class CMyCURLPL {

public:

  CMyCURLPL()
  {
    curlRet = "";
    list = NULL;
  }
  ~CMyCURLPL() {}

  static CMyCURLPL *
  getInstance();

public:

  static size_t
  curlResponseCallback(char * data, size_t size, size_t nmemb, void * ptr);

public:

  int
  curlInit();

  CURL *
  curlInstance();

public:

  int
  curlBefore(CQIHolder * holder);

  int
  curlPost(CQIHolder * holder, const char * url, const char * postheader, const char * postdata);

  int
  curlGet(CQIHolder * holder, const char * url);

  int
  curlAfter(CQIHolder * holder);

  int
  curlResponseCode(CQIHolder * holder, int & output);

  int
  curlResponse(CQIHolder * holder, std::string & output);

public:

  int
  doHTTPPost(CQIHolder * holder,
             std::string & sURL,
             std::string & sHeader,
             std::string & sBody,
             int & responseCode,
             std::string & response);

public:
  std::string curlRet;

private:
  static CMyCURLPL * instance;
  std::mutex mtx;
  std::mutex curlmtx;
  struct curl_slist * list;
};

}

#endif
