#ifndef MYOPENSSL_HPP
#define MYOPENSSL_HPP

#include <string.h>
#include <stdio.h>

#include <sys/time.h>
#include <time.h>

#include <openssl/sha.h>
#include <openssl/crypto.h>

#include <string>
#include <iostream>
//begin 20170601 liulijin add
#include "../logging4.h"
//end 20170601 liulijin add
using namespace std;

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMySSLResultException : public CMy_BSDR_Exception
{
public:
  CMySSLResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CMySSLResultException() {};
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

class CMyOpenSSL {

public:

  CMyOpenSSL()
  {
    isTimeOff = true;
  }
  ~CMyOpenSSL() {}

  static CMyOpenSSL *
  getInstance();

public:

  inline bool sha256compare(const std::string & src,
         std::string & c2){
         std::string output("");
         SHA256(src,output);
         if(src==c2)
         {
           return true;
         }
        return false;
  }

  int
  SHA256(const std::string & input,
         std::string & output);
  int
  getTimeStamp(unsigned long * t);

  int
  getTimeStamp2(char ** t);

  int
  printTimeNow();

  int
  printTimeBegin(unsigned long * t);

  int
  printTimeBeginV2(unsigned long * t);

  int
  printTimeEnd(unsigned long lbegin);

  int
  printTimeEndV2(unsigned long lbegin);

  int
  printTimeNowByFormatted();

  int
  hash_string(std::string string_piece);
  

  int
  hash_string2(std::string string_piece);

  std::string 
  intToString(size_t ori);
  
  int
  setTimeOffOn(bool offon)
  {
    isTimeOff = offon;

    return 0;
  }

  int
  getFakeRandomUInt(const unsigned int istart,
                    const unsigned int iend,
                    unsigned int & iresult);

private:
  static CMyOpenSSL * instance;

  bool isTimeOff;
};

}

#endif
