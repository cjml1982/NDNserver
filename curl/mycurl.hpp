
#ifndef MYCURL_HPP
#define MYCURL_HPP

#include <string.h>

#include <curl/curl.h>

#include <string>
#include <iostream>
#include <thread>
#include <mutex>

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMyCurlResultException : public CMy_BSDR_Exception
{
public:
  CMyCurlResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CMyCurlResultException() {};
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

class CMyCURL {

public:

  CMyCURL()
  {
    curlRet = "";
    list = NULL;
  }
  ~CMyCURL() {}

  static CMyCURL *
  getInstance();

public:

  static size_t
  curlResponseCallback(char * data, size_t size, size_t nmemb, void * ptr);

  int
  curlInit();

  int
  curlInstance();

  int
  curlBefore();

  int
  curlPost(const char * url, const char * postheader, const char * postdata);

  int
  curlGet(const char * url);

  int
  curlAfter();

  int
  curlResponseCode(int & output);

  int
  curlResponse(std::string & output);

  int
  curlDestroy();

  int
  doHTTPPost(std::string & sURL,
             std::string & sHeader,
             std::string & sBody,
             int & responseCode,
             std::string & response);

  std::string curlRet;
private:
  static CMyCURL * instance;
  CURL * curl;
  struct curl_slist * list;
  std::mutex curlmtx;
  
};

}

#endif
