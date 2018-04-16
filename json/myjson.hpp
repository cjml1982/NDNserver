
#ifndef MYJSON_HPP
#define MYJSON_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include <jsoncpp/json/json.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/allocators.h>

using namespace std;
using namespace rapidjson;

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMyJsonResultException : public CMy_BSDR_Exception
{
public:
  CMyJsonResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception(info, g_rescode)
  {
  }
  virtual ~CMyJsonResultException() {};
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

class CMyJSON {

public:

  CMyJSON() {}
  ~CMyJSON() {}

  static CMyJSON *
  getInstance();

public:

  int
  stringIsANumber(const std::string & input);

  int
  stringIsBeginEnd(const std::string & input);

  int
  stringSplit(const std::string & input,
              std::string & output);

  int
  stringSplitB(const std::string & input,
               std::string & output);

  int
  charOutOfHead(const std::string & input,
                std::string & output);

  int
  charOutOfTail(const std::string & input,
                std::string & output);

  int
  charOutOfHeadTail(const std::string & input,
                    std::string & output);

  int
  stringSearthRemaining(const std::string & source,
                        const std::string & search,
                        std::string & remain);

  int
  writeJSON(const std::string & methodValue,
            const std::string & paramsValue,
            std::string & json);

  int
  writeJSONArray(const std::string & methodValue,
                 const std::vector<std::string> & arrayValue,
                 std::string & json);

  int
  writeJSONArraySendToAddress(const std::string & methodValue,
                              const std::vector<std::string> & arrayValue,
                              std::string & json);

  int
  writeJSONArraySendBnRawTransaction(const std::string & methodValue,
                                     const std::vector<std::string> & arrayValue,
                                     std::string & json);

  int
  writeJSONArrayGetBnRawTransaction(const std::string & methodValue,
                                    const std::vector<std::string> & arrayValue,
                                    std::string & json);

  int
  writeJSONGetBalance(const std::string & methodValue,
                      const std::string & paramsValue,
                      std::string & json);

  int
  readJSON(const std::string & input,
           std::string & result,
           std::string & error,
           std::string & id);

  int
  readJSONSendToAddress(const std::string & input,
                        std::string & result,
                        std::string & error,
                        std::string & id);

  int
  readJSONSendBnRawTransaction(const std::string & input,
                               std::string & result,
                               std::string & txid,
                               std::string & reqseq,
                               std::string & error,
                               std::string & id);

  int
  readJSONGetBnRawTransaction(const std::string & input,
                              std::string & result,
                              std::string & error,
                              std::string & id);

  int
  readJSONGetBalance(const std::string & input,
                     std::string & result,
                     std::string & error,
                     std::string & id);

  int writeJSONS3File(const string & user,
		  	  	  	   const string & host,
		  	  	  	   const string & access,
                   const string & secret,
                   const string & bucket,
                   const string & key,
					         string & json);
typedef unsigned char BYTE;  
   
    inline BYTE toHex(const BYTE &x)  
    {  
        return x > 9 ? x -10 + 'A': x + '0';  
    }  
  
    inline BYTE fromHex(const BYTE &x)  
    {  
        return isdigit(x) ? x-'0' : x-'A'+10;  
    }  
   
    inline string URLEncode(const string &sIn)  
    {  
        string sOut;  
        for( size_t ix = 0; ix < sIn.size(); ix++ )  
        {        
            BYTE buf[4];  
            memset( buf, 0, 4 );  
            if( isalnum( (BYTE)sIn[ix] ) )  
            {        
                buf[0] = sIn[ix];  
            }  
            //else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以  
            //{  
            //    buf[0] = '+';  
            //}  
            else  
            {  
                buf[0] = '%';  
                buf[1] = toHex( (BYTE)sIn[ix] >> 4 );  
                buf[2] = toHex( (BYTE)sIn[ix] % 16);  
            }  
            sOut += (char *)buf;  
        }  
        return sOut;  
    };  
  
    inline string URLDecode(const string &sIn)  
    {  
        string sOut;  
        for( size_t ix = 0; ix < sIn.size(); ix++ )  
        {  
            BYTE ch = 0;  
            if(sIn[ix]=='%')  
            {  
                ch = (fromHex(sIn[ix+1])<<4);  
                ch |= fromHex(sIn[ix+2]);  
                ix += 2;  
            }  
            // else if(sIn[ix] == '+')  
            // {  
            //     ch = ' ';  
            // }  
            else  
            {  
                ch = sIn[ix];  
            }  
            sOut += (char)ch;  
        }  
        return sOut;  
    }  


private:
  static CMyJSON * instance;
};

}

#endif
