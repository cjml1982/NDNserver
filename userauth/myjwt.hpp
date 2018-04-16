
#ifndef MYJWT_HPP
#define MYJWT_HPP

#include <utility>
#include <string.h>

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "../curl/mycurl.hpp"
#include "../curl/mycurlpl.hpp"
#include "../myholder.hpp"
#include "../constant/myresultcode.hpp"

class CJWTResultException : public CMy_BSDR_Exception
{
public:
  CJWTResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CJWTResultException() {};
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

//JWT
enum class JWT_TYPE : int
{
  JWT_TYPE_AUTHREGISTER = -1,
  JWT_TYPE_AUTHAUTHREQUEST = -2,
  JWT_TYPE_AUTHRESPONSE = -3,
  JWT_TYPE_AUTHVERIFY = -4
};

//JWT client configuration
const static std::string ASYM_AUTH_SCHEMA{"http://"};
const static std::string ASYM_AUTH_SYMBOL_COLON{":"};
const static std::string ASYM_AUTH_SYMBOL_SLASH{"/"};
const static std::string ASYM_AUTH_REGISTER{"asym-auth/register/"};
const static std::string ASYM_AUTH_AUTHREQUEST{"asym-auth/authRequest/"};
const static std::string ASYM_AUTH_RESPONSE{"asym-auth/response/"};
const static std::string ASYM_AUTH_VERIFY{"asym-auth/verify/"};

const static std::string ASYM_AUTH_JWT_KEY_USER_ID{"user"};
const static std::string ASYM_AUTH_JWT_KEY_USER_PUBKEY{"publickey"};
const static std::string ASYM_AUTH_JWT_KEY_USER_SIG{"signature"};
const static std::string ASYM_AUTH_JWT_KEY_USER_AUTHS{"Authorization"};
const static std::string ASYM_AUTH_JWT_SYMBOL_EQUALS{"="};
const static std::string ASYM_AUTH_JWT_SYMBOL_JOINER{"&"};

struct CJWTInputIPPort
{
  std::string ip;
  std::string port;
};

struct CJWTInputHeader
{
  std::string hdr;
};

struct CJWTInputParams
{
  std::string user_id;
  std::string user_pubkey;
  std::string user_sig;
  std::string user_authtoken;
};

class CJWTClient
{
public:
  CJWTClient() {}
  ~CJWTClient() {}

  static CJWTClient *
  getInstance();

public:
  int
  doJWTComm(JWT_TYPE type,
            ndn::CQIHolder * holder,
            CJWTInputIPPort & ipport,
            CJWTInputHeader & hdr,
            CJWTInputParams & params,
            int & resCode,
            std::string & res)
  {
    int ret = 0;

    std::string posturl{  ASYM_AUTH_SCHEMA
                        + ipport.ip
                        + ASYM_AUTH_SYMBOL_COLON
                        + ipport.port
                        + ASYM_AUTH_SYMBOL_SLASH};
    std::string postheader{hdr.hdr};
    std::string postdata{""};

    switch (type)
    {
      case JWT_TYPE::JWT_TYPE_AUTHREGISTER:
        posturl.append(ASYM_AUTH_REGISTER);
        postdata = (  ASYM_AUTH_JWT_KEY_USER_ID
                    + ASYM_AUTH_JWT_SYMBOL_EQUALS
                    + params.user_id
                    + ASYM_AUTH_JWT_SYMBOL_JOINER
                    + ASYM_AUTH_JWT_KEY_USER_PUBKEY
                    + ASYM_AUTH_JWT_SYMBOL_EQUALS
                    + params.user_pubkey);
        break;
      case JWT_TYPE::JWT_TYPE_AUTHAUTHREQUEST:
        posturl.append(ASYM_AUTH_AUTHREQUEST);
        postdata.append(  ASYM_AUTH_JWT_KEY_USER_ID
                        + ASYM_AUTH_JWT_SYMBOL_EQUALS
                        + params.user_id);
        break;
      case JWT_TYPE::JWT_TYPE_AUTHRESPONSE:
        posturl.append(ASYM_AUTH_RESPONSE);
        postdata.append(  ASYM_AUTH_JWT_KEY_USER_ID
                        + ASYM_AUTH_JWT_SYMBOL_EQUALS
                        + params.user_id
                        + ASYM_AUTH_JWT_SYMBOL_JOINER
                        + ASYM_AUTH_JWT_KEY_USER_SIG
                        + ASYM_AUTH_JWT_SYMBOL_EQUALS
                        + params.user_sig);
        break;
      case JWT_TYPE::JWT_TYPE_AUTHVERIFY:
        posturl.append(ASYM_AUTH_VERIFY);
        postdata.append(  ASYM_AUTH_JWT_KEY_USER_ID
                        + ASYM_AUTH_JWT_SYMBOL_EQUALS
                        + params.user_id
                        + ASYM_AUTH_JWT_SYMBOL_JOINER
                        + ASYM_AUTH_JWT_KEY_USER_AUTHS
                        + ASYM_AUTH_JWT_SYMBOL_EQUALS
                        + params.user_authtoken);
        break;
      default: break;
    }
    //michael 20180307 modified
    if (nullptr == holder)
    {
      ndn::CMyCURL::getInstance()->doHTTPPost(posturl, postheader, postdata, resCode, res);
    }
    else
    {
      ndn::CMyCURLPL::getInstance()->doHTTPPost(holder, posturl, postheader, postdata, resCode, res);
    }

    return ret;
  }

private:
  static CJWTClient * instance;
};

//parser
using INT_BIG = long long int;

struct CJWTResult
{
  std::pair<std::string, std::string> result;
};

struct CJWTMessage
{
  std::pair<std::string, std::string> message;
  std::pair<std::string, std::string> message_name;
  //std::pair<std::string, INT_BIG> message_random;
  std::pair<std::string, std::string> message_random;
  std::pair<std::string, std::string> message_JWT;
};

struct CJWTError
{
  std::pair<std::string, std::string> error;
  std::pair<std::string, int> error_code;
  std::pair<std::string, std::string> error_info;
};

class CJWTResponse
{
public:
  CJWTResponse() {};
  CJWTResponse(const CJWTResult & jwtr,
               const CJWTMessage & jwtm,
               const CJWTError & jwte):m_jwtr(jwtr),m_jwtm(jwtm),m_jwte(jwte)
  {
  }
  ~CJWTResponse() {}

public:
  CJWTResult getJWTResult(void)
  {
    return m_jwtr;
  }

  CJWTMessage getJWTMessage(void)
  {
    return m_jwtm;
  }

  CJWTError getJWTError(void)
  {
    return m_jwte;
  }

  void setJWTResult(CJWTResult & jwtr)
  {
    m_jwtr = jwtr;
  }

  void setJWTMessage(CJWTMessage & jwtm)
  {
    m_jwtm = jwtm;
  }

  void setJWTError(CJWTError & jwte)
  {
    m_jwte = jwte;
  }

private:
  CJWTResult m_jwtr;
  CJWTMessage m_jwtm;
  CJWTError m_jwte;
};

const static std::string JWT_KEY_RESULT = "result";
const static std::string JWT_KEY_MESSAGE = "message";
const static std::string JWT_KEY_ERROR = "error";
const static std::string JWT_KEY_NAME = "name";
const static std::string JWT_KEY_RANDOM = "random";
const static std::string JWT_KEY_JWT = "JWT";
const static std::string JWT_KEY_INFO = "info";
const static std::string JWT_KEY_CODE = "code";

const static std::string JWT_VALUE_SUCCESS = "success";
const static std::string JWT_VALUE_ERROR = "error";
const static std::string JWT_VALUE_NULL = "null";

enum JWT_ERR_CODE
{
  JWT_ERR_CODE_AUTHREGISTER_1 = -101,
  JWT_ERR_CODE_AUTHREGISTER_2 = -102,
  JWT_ERR_CODE_AUTHAUTHREQUEST_1 = -201,
  JWT_ERR_CODE_AUTHRESPONSE_1 = -301,
  JWT_ERR_CODE_AUTHRESPONSE_2 = -302,
  JWT_ERR_CODE_AUTHVERIFY_1 = -401,
  JWT_ERR_CODE_AUTHVERIFY_2 = -402,
  JWT_ERR_CODE_AUTHVERIFY_3 = -403,
};
//enum class JWT_ERR_CODE : int
//{
//  JWT_ERR_CODE_AUTHREGISTER_1 = -101,
//  JWT_ERR_CODE_AUTHREGISTER_2 = -102,
//  JWT_ERR_CODE_AUTHAUTHREQUEST_1 = -201,
//  JWT_ERR_CODE_AUTHRESPONSE_1 = -301,
//  JWT_ERR_CODE_AUTHRESPONSE_2 = -302,
//  JWT_ERR_CODE_AUTHVERIFY_1 = -401,
//  JWT_ERR_CODE_AUTHVERIFY_2 = -402,
//  JWT_ERR_CODE_AUTHVERIFY_3 = -403
//};

class CJWTParser
{
public:
  CJWTParser() {}
  ~CJWTParser() {}

  static CJWTParser *
  getInstance();

public:
  int
  parseJWTAuthBlob(JWT_TYPE type,
                   std::string & in,
                   CJWTResponse & jwtres)
  {
    int ret = 0;

    switch (type)
    {
      case JWT_TYPE::JWT_TYPE_AUTHREGISTER:
        ret = parseAuthRegister(in, jwtres);
        break;
      case JWT_TYPE::JWT_TYPE_AUTHAUTHREQUEST:
        ret = parseAuthAuthRequest(in, jwtres);
        break;
      case JWT_TYPE::JWT_TYPE_AUTHRESPONSE:
        ret = parseAuthResponse(in, jwtres);
        break;
      case JWT_TYPE::JWT_TYPE_AUTHVERIFY:
        ret = parseAuthVerify(in, jwtres);
        break;
      default: break;
    }

    switch((JWT_ERR_CODE)jwtres.getJWTError().error_code.second)
    {
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHREGISTER_1:
        throw new CJWTResultException{"User already registered", __GLOBAL_RESULT_JWT_USER_ALREADY_REG};
        break;
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHREGISTER_2:
        throw new CJWTResultException{"Public key format error", __GLOBAL_RESULT_JWT_USER_PUBKEY_FMT_ERR};
        break;
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHAUTHREQUEST_1:
        throw new CJWTResultException{"User not exist", __GLOBAL_RESULT_JWT_USER_NOT_EXIST};
        break;
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHRESPONSE_1:
        throw new CJWTResultException{"Verify the signature fail", __GLOBAL_RESULT_JWT_USER_SIG_FAIL};
        break;
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHRESPONSE_2:
        throw new CJWTResultException{"Challenge number not exist", __GLOBAL_RESULT_JWT_USER_CHALLENGE_NUMBER_ERR};
        break;
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHVERIFY_1:
        throw new CJWTResultException{"JWT verify fail", __GLOBAL_RESULT_JWT_USER_AUTH_VERIFY_FAIL};
        break;
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHVERIFY_2:
        throw new CJWTResultException{"JWT expired", __GLOBAL_RESULT_JWT_USER_AUTH_EXPIRED};
        break;
      case JWT_ERR_CODE::JWT_ERR_CODE_AUTHVERIFY_3:
        throw new CJWTResultException{"User has no permissions", __GLOBAL_RESULT_JWT_USER_AUTH_NO_PERMISSIONS};
        break;
      default: break;
    }

    return ret;
  }

private:
  int
  parseAuthRegister(std::string & in,
                    CJWTResponse & jwtres)
  {
    CJWTResult jwtr;
    CJWTMessage jwtm;
    CJWTError jwte;
    rapidjson::Document doc;
    doc.Parse(in.c_str());

    const rapidjson::Value & obj1 = doc[JWT_KEY_RESULT.c_str()];

    if (obj1.GetString() == JWT_VALUE_SUCCESS)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_SUCCESS;

      const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      const rapidjson::Value & obj3 = obj2[JWT_KEY_NAME.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message_name.first = JWT_KEY_NAME;
      jwtm.message_name.second = obj3.GetString();

      //const rapidjson::Value & obj4 = doc[JWT_KEY_ERROR.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error.second = JWT_VALUE_NULL;
    }
    else if (obj1.GetString() == JWT_VALUE_ERROR)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_ERROR;

      //const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message.second = JWT_VALUE_NULL;

      const rapidjson::Value & obj3 = doc[JWT_KEY_ERROR.c_str()];
      const rapidjson::Value & obj4 = obj3[JWT_KEY_INFO.c_str()];
      const rapidjson::Value & obj5 = obj3[JWT_KEY_CODE.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error_info.first = JWT_KEY_INFO;
      jwte.error_info.second = obj4.GetString();
      jwte.error_code.first = JWT_KEY_CODE;
      jwte.error_code.second = obj5.GetInt();
    }

    CJWTResponse res{jwtr, jwtm, jwte};
    jwtres = res;
    return 0;
  }

  int
  parseAuthAuthRequest(std::string & in,
                       CJWTResponse & jwtres)
  {
    CJWTResult jwtr;
    CJWTMessage jwtm;
    CJWTError jwte;
    rapidjson::Document doc;
    doc.Parse(in.c_str());

    const rapidjson::Value & obj1 = doc[JWT_KEY_RESULT.c_str()];

    if (obj1.GetString() == JWT_VALUE_SUCCESS)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_SUCCESS;

      const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      const rapidjson::Value & obj3 = obj2[JWT_KEY_NAME.c_str()];
      const rapidjson::Value & obj4 = obj2[JWT_KEY_RANDOM.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message_name.first = JWT_KEY_NAME;
      jwtm.message_name.second = obj3.GetString();
      jwtm.message_random.first = JWT_KEY_RANDOM;
      //jwtm.message_random.second = obj4.GetInt64();
      jwtm.message_random.second = obj4.GetString();

      //const rapidjson::Value & obj5 = doc[JWT_KEY_ERROR.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error.second = JWT_VALUE_NULL;
    }
    else if (obj1.GetString() == JWT_VALUE_ERROR)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_ERROR;

      //const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message.second = JWT_VALUE_NULL;

      const rapidjson::Value & obj3 = doc[JWT_KEY_ERROR.c_str()];
      const rapidjson::Value & obj4 = obj3[JWT_KEY_INFO.c_str()];
      const rapidjson::Value & obj5 = obj3[JWT_KEY_CODE.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error_info.first = JWT_KEY_INFO;
      jwte.error_info.second = obj4.GetString();
      jwte.error_code.first = JWT_KEY_CODE;
      jwte.error_code.second = obj5.GetInt();
    }

    CJWTResponse res{jwtr, jwtm, jwte};
    jwtres = res;
    return 0;
  }

  int
  parseAuthResponse(std::string & in,
                    CJWTResponse & jwtres)
  {
    CJWTResult jwtr;
    CJWTMessage jwtm;
    CJWTError jwte;
    rapidjson::Document doc;
    doc.Parse(in.c_str());

    const rapidjson::Value & obj1 = doc[JWT_KEY_RESULT.c_str()];

    if (obj1.GetString() == JWT_VALUE_SUCCESS)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_SUCCESS;

      const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      const rapidjson::Value & obj3 = obj2[JWT_KEY_NAME.c_str()];
      const rapidjson::Value & obj4 = obj2[JWT_KEY_JWT.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message_name.first = JWT_KEY_NAME;
      jwtm.message_name.second = obj3.GetString();
      jwtm.message_JWT.first = JWT_KEY_JWT;
      jwtm.message_JWT.second = obj4.GetString();

      //const rapidjson::Value & obj5 = doc[JWT_KEY_ERROR.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error.second = JWT_VALUE_NULL;
    }
    else if (obj1.GetString() == JWT_VALUE_ERROR)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_ERROR;

      //const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message.second = JWT_VALUE_NULL;

      const rapidjson::Value & obj3 = doc[JWT_KEY_ERROR.c_str()];
      const rapidjson::Value & obj4 = obj3[JWT_KEY_INFO.c_str()];
      const rapidjson::Value & obj5 = obj3[JWT_KEY_CODE.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error_info.first = JWT_KEY_INFO;
      jwte.error_info.second = obj4.GetString();
      jwte.error_code.first = JWT_KEY_CODE;
      jwte.error_code.second = obj5.GetInt();
    }

    CJWTResponse res{jwtr, jwtm, jwte};
    jwtres = res;
    return 0;
  }

  int
  parseAuthVerify(std::string & in,
                  CJWTResponse & jwtres)
  {
    CJWTResult jwtr;
    CJWTMessage jwtm;
    CJWTError jwte;
    rapidjson::Document doc;
    doc.Parse(in.c_str());

    const rapidjson::Value & obj1 = doc[JWT_KEY_RESULT.c_str()];

    if (obj1.GetString() == JWT_VALUE_SUCCESS)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_SUCCESS;

      const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      const rapidjson::Value & obj3 = obj2[JWT_KEY_NAME.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message_name.first = JWT_KEY_NAME;
      jwtm.message_name.second = obj3.GetString();

      //const rapidjson::Value & obj4 = doc[JWT_KEY_ERROR.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error.second = JWT_VALUE_NULL;
    }
    else if (obj1.GetString() == JWT_VALUE_ERROR)
    {
      jwtr.result.first = JWT_KEY_RESULT;
      jwtr.result.second = JWT_VALUE_ERROR;

      //const rapidjson::Value & obj2 = doc[JWT_KEY_MESSAGE.c_str()];
      jwtm.message.first = JWT_KEY_MESSAGE;
      jwtm.message.second = JWT_VALUE_NULL;

      const rapidjson::Value & obj3 = doc[JWT_KEY_ERROR.c_str()];
      const rapidjson::Value & obj4 = obj3[JWT_KEY_INFO.c_str()];
      const rapidjson::Value & obj5 = obj3[JWT_KEY_CODE.c_str()];
      jwte.error.first = JWT_KEY_ERROR;
      jwte.error_info.first = JWT_KEY_INFO;
      jwte.error_info.second = obj4.GetString();
      jwte.error_code.first = JWT_KEY_CODE;
      jwte.error_code.second = obj5.GetInt();
    }

    CJWTResponse res{jwtr, jwtm, jwte};
    jwtres = res;
    return 0;
  }

private:
  static CJWTParser * instance;
};

#endif
