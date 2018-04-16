
#include "myjwt.hpp"
#include "../logging4.h"

INIT_LOGGER("CMyJWT");

CJWTClient * CJWTClient::instance = new CJWTClient;

CJWTClient *
CJWTClient::getInstance()
{
  return instance;
}

CJWTParser * CJWTParser::instance = new CJWTParser;

CJWTParser *
CJWTParser::getInstance()
{
  return instance;
}
