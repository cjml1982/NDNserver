/*
 * filerw.h
 *
 *  Created on: 2018年3月1日
 *      Author: liaoyp
 */

#ifndef S3FILE_H_
#define S3FILE_H_

#include "s3/s3.h"
#include <string>

//michael 20180307 added
using namespace std;
#include <iostream>
#include "constant/myresultcode.hpp"
enum class S3_ACTION_TYPE : int
{
  PUT_FILE = -1,
  GET_FILE = -2,
  DEL_FILE = -3,
  UPD_FILE = -4
};

using namespace std;
#include <iostream>
#include "constant/myresultcode.hpp"
class CS3ResultException : public CMy_BSDR_Exception
{
public:
  CS3ResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CS3ResultException() {};
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

static const unsigned long POSITION1 = 10 * 1024 * 1024;
static const unsigned long POSITION2 = 100 * 1024 * 1024;
static const string SB = "SmallBucket";
static const string MB = "MiddleBucket";
static const string LB = "LargeBucket";

using namespace std;

class S3File
{
private:
	enum TYPE {SMALL, MIDDLE, LARGE};

public:
	int putFile(const string & srcfilename, const uint64_t size, string & result);
	int getFile(const string & dstfile, const string & bucket, string & result);
	int deleteFile(const string & dstfile, const string & bucket);
	int updateFile(const string & srcfilename, const uint64_t size, const string & dstfile, const string & bucket, string & result);
public:
	int doCRUDS3File(S3_ACTION_TYPE type, const string & srcfile,const uint64_t size, 
	                 const string & dstfile, const string & bucket, string & result)
	{
	  int ret = 0;
	  int s3Status = 0;
    switch (type)
    {
      case S3_ACTION_TYPE::PUT_FILE:
        s3Status = putFile(srcfile,size, result);
        break;
      case S3_ACTION_TYPE::GET_FILE:
        s3Status = getFile(dstfile, bucket, result);
        break;
      case S3_ACTION_TYPE::DEL_FILE:
        s3Status = deleteFile(dstfile, bucket);
        break;
      case S3_ACTION_TYPE::UPD_FILE:
        s3Status = updateFile(srcfile, size,dstfile, bucket, result);
        break;
      default: break;
	  }

    switch (s3Status)
    {
      case StatusInvalidArgument:
        throw new CS3ResultException{"s3 invalid argument", __GLOBAL_RESULT_S3_INVALID_ARGUMENT};
        break;
      case StatusRequestFailed:
        throw new CS3ResultException{"s3 request failed", __GLOBAL_RESULT_S3_REQUEST_FAILED};
        break;
      case StatusResponseFailed:
        throw new CS3ResultException{"s3 response failed", __GLOBAL_RESULT_S3_RESPONSE_FAILED};
        break;
      case StatusXmlParseFailure:
        throw new CS3ResultException{"s3 xml parse failure", __GLOBAL_RESULT_S3_XML_PARSE_FAILURE};
        break;
      case StatusFileObtainFailed:
        throw new CS3ResultException{"s3 file obtain failed", __GLOBAL_RESULT_S3_FILE_ABTAIN_FAILED};
        break;
      case StatusOutOfMemory:
        throw new CS3ResultException{"s3 out of memory", __GLOBAL_RESULT_S3_OUT_OF_MEMORY};
        break;
      case StatusWriteJsonFailed:
        throw new CS3ResultException{"s3 write json failed", __GLOBAL_RESULT_S3_WRITE_JSON_FAILED};
        break;
      default: break;
    }

    return ret;
	}
};

//michael 20180307
#include <utility>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;
using namespace std;

const static std::string __S3_KEY_S3HOST = "S3Host";
const static std::string __S3_KEY_S3USER = "S3User";
const static std::string __S3_KEY_S3BUCKET = "S3Bucket";
const static std::string __S3_KEY_S3ACCESS = "S3Access";
const static std::string __S3_KEY_S3SECRET = "S3Secret";
const static std::string __S3_KEY_S3DSTFILE = "S3DstFile";
const static std::string __S3_KEY_S3SRCFILE = "S3SrcFile";

class CS3_Result
{
public:
  CS3_Result() {};
  CS3_Result(const std::pair<std::string, std::string> & s3host,
             const std::pair<std::string, std::string> & s3user,
             const std::pair<std::string, std::string> & s3access,
             const std::pair<std::string, std::string> & s3secret,
             const std::pair<std::string, std::string> & s3bucket,
             const std::pair<std::string, std::string> & s3dstfile)
             :m_s3host(s3host), m_s3user(s3user), m_s3access(s3access)
             ,m_s3secret(s3secret), m_s3bucket(s3bucket), m_s3dstfile(s3dstfile)
  {
  }
  ~CS3_Result() {}

public:
  std::pair<std::string, std::string>
  getS3Host(void)
  {
    return m_s3host;
  }

  std::pair<std::string, std::string>
  getS3User(void)
  {
    return m_s3user;
  }

  std::pair<std::string, std::string>
  getS3Access(void)
  {
    return m_s3access;
  }

  std::pair<std::string, std::string>
  getS3Secret(void)
  {
    return m_s3secret;
  }

  std::pair<std::string, std::string>
  getS3Bucket(void)
  {
    return m_s3bucket;
  }

  std::pair<std::string, std::string>
  getS3DstFile(void)
  {
    return m_s3dstfile;
  }

  std::pair<std::string, std::string>
  getS3SrcFile(void)
  {
    return m_s3srcfile;
  }

  void setS3Host(std::pair<std::string, std::string> & s3host)
  {
    m_s3host = s3host;
  }

  void setS3User(std::pair<std::string, std::string> & s3user)
  {
    m_s3user = s3user;
  }

  void setS3Access(std::pair<std::string, std::string> & s3access)
  {
    m_s3access = s3access;
  }

  void setS3Secret(std::pair<std::string, std::string> & s3secret)
  {
    m_s3secret = s3secret;
  }

  void setS3Bucket(std::pair<std::string, std::string> & s3bucket)
  {
    m_s3bucket = s3bucket;
  }

  void setS3DstFile(std::pair<std::string, std::string> & s3dstfile)
  {
    m_s3dstfile = s3dstfile;
  }

  void setS3SrcFile(std::pair<std::string, std::string> & s3srcfile)
  {
    m_s3srcfile = s3srcfile;
  }

private:
  std::pair<std::string, std::string> m_s3host;
  std::pair<std::string, std::string> m_s3user;
  std::pair<std::string, std::string> m_s3access;
  std::pair<std::string, std::string> m_s3secret;
  std::pair<std::string, std::string> m_s3bucket;
  std::pair<std::string, std::string> m_s3dstfile;
  std::pair<std::string, std::string> m_s3srcfile;
};

class CS3_Result_Parser
{
public:
  CS3_Result_Parser() {}
  ~CS3_Result_Parser() {}

  static CS3_Result_Parser *
  getInstance();

public:
  int
  parseS3Blob(S3_ACTION_TYPE type,
              std::string & in,
              CS3_Result & s3res)
  {
    int ret = 0;

    switch (type)
    {
      case S3_ACTION_TYPE::PUT_FILE:
        ret = parseS3PUT(in, s3res);
        break;
      case S3_ACTION_TYPE::GET_FILE:
        ret = parseS3GET(in, s3res);
        break;
      case S3_ACTION_TYPE::DEL_FILE:
        break;
      case S3_ACTION_TYPE::UPD_FILE:
        ret = parseS3UPD(in, s3res);
        break;
      default: break;
    }

    return ret;
  }

private:
  int
  parseS3PUT(std::string & in,
             CS3_Result & s3res)
  {
    return parseS3Result(in, s3res);
  }

  int
  parseS3GET(std::string & in,
             CS3_Result & s3res)
  {
    return parseS3Result(in, s3res);
  }

  int
  parseS3UPD(std::string & in,
             CS3_Result & s3res)
  {
    return parseS3Result(in, s3res);
  }

private:

  int parseS3Result(std::string & in,
                    CS3_Result & s3res)
  {
    rapidjson::Document doc;
    doc.Parse(in.c_str());

    const rapidjson::Value & obj1 = doc[__S3_KEY_S3HOST.c_str()];
    std::pair<std::string, std::string> s3host{__S3_KEY_S3HOST, obj1.GetString()};

    const rapidjson::Value & obj2 = doc[__S3_KEY_S3USER.c_str()];
    std::pair<std::string, std::string> s3user{__S3_KEY_S3USER, obj2.GetString()};

    const rapidjson::Value & obj3 = doc[__S3_KEY_S3ACCESS.c_str()];
    std::pair<std::string, std::string> s3access{__S3_KEY_S3ACCESS, obj3.GetString()};

    const rapidjson::Value & obj4 = doc[__S3_KEY_S3SECRET.c_str()];
    std::pair<std::string, std::string> s3secret{__S3_KEY_S3SECRET, obj4.GetString()};

    const rapidjson::Value & obj5 = doc[__S3_KEY_S3BUCKET.c_str()];
    std::pair<std::string, std::string> s3bucket{__S3_KEY_S3BUCKET, obj5.GetString()};

    const rapidjson::Value & obj6 = doc[__S3_KEY_S3DSTFILE.c_str()];
    std::pair<std::string, std::string> s3dstfile{__S3_KEY_S3DSTFILE, obj6.GetString()};
    //const rapidjson::Value & obj7 = doc[__S3_KEY_S3SRCFILE.c_str()];

    CS3_Result res{s3host, s3user, s3access, s3secret, s3bucket, s3dstfile};
    s3res = res;

    return 0;
  }

private:
  static CS3_Result_Parser * instance;
};

#endif /* S3FILE_H_ */
