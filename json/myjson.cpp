
#include "myjson.hpp"
//begin 20170601 liulijin add
#include "../logging4.h"
#include "../openssl/myopenssl.hpp"
#include "../constant/myconstant.hpp"
//end 20170601 liulijin add

INIT_LOGGER("CMyJSON");

ndn::CMyJSON * ndn::CMyJSON::instance = new CMyJSON;

ndn::CMyJSON *
ndn::CMyJSON::getInstance()
{
  return instance;
}

int
ndn::CMyJSON::stringIsANumber(const std::string & input)
{
  _LOG_INFO("begin stringIsANumber");

  std::string in(input);
  char s[256] = {0};
  snprintf(s, sizeof(s), "%s", in.c_str());

  unsigned int i;
  unsigned int j = 0;
  unsigned int k = 0;
  for (i = 0; s[i] != 0; i++)
  {
    if (isdigit(s[i]))
    {
      j++;
    }
    if (s[i] == '.')
    {
      k++;
    }
  }

 //fprintf(stderr, "i=%u,j=%u,k=%u\n",i,j,k);
  _LOG_INFO("i=" << i << ",j=" << j << ",k=" << k);

  if (k > 1)
  {
    //throw new CMyJsonResultException{"stringIsANumber", __GLOBAL_RESULT_JSON_STRING_IS_NOT_NUMBER};
    return -1;
  }

  if (i != (j+k))
  {
    //throw new CMyJsonResultException{"stringIsANumber", __GLOBAL_RESULT_JSON_STRING_IS_NOT_NUMBER};
    return -1;
  }

  _LOG_INFO("end stringIsANumber");
  return 0;
}

int
ndn::CMyJSON::stringIsBeginEnd(const std::string & input)
{
  _LOG_INFO("begin stringIsBeginEnd");

  std::string in(input);
  //blockedlabel as big string
  char * s = (char *)malloc(sizeof(char *) * 1024);
  sprintf(s, "%s", in.c_str());

  if (s[0] != '/')
  {
   //fprintf(stderr, "Error! It is not begining with '/'\n");
    free(s);//note
    _LOG_INFO("Error! It is not begining with '/'");

    //throw new CMyJsonResultException{"stringIsBeginEnd", __GLOBAL_RESULT_JSON_STRING_IS_NOT_BEGINEND};
    return -1;
  }

  if (s[in.length()-1] != '/')
  {
   //fprintf(stderr, "Error! It is ending with '/'\n");
    free(s);//note
    _LOG_INFO("Error! It is ending with '/'");

    //throw new CMyJsonResultException{"stringIsBeginEnd", __GLOBAL_RESULT_JSON_STRING_IS_NOT_BEGINEND};
    return -1;
  }

  free(s);//note
  _LOG_INFO("end stringIsBeginEnd");
  return 0;
}

int
ndn::CMyJSON::stringSplit(const std::string & input,
                          std::string & output)
{
  _LOG_INFO("begin stringSplit");

  char delim = ',';

  std::stringstream ss;
  ss.str(input);
  std::string item;

  std::vector<std::string> array;
  array.clear();

  while (std::getline(ss, item, delim))
  {
    array.push_back(item);
  }

  for (unsigned int i = 0; i < array.size(); i++)
  {
    //cout << array[i] << endl;
    _LOG_INFO("array: " << array[i]);
  }

  output = array[1];

  _LOG_INFO("end stringSplit");
  return 0;
}

int
ndn::CMyJSON::stringSplitB(const std::string & input,
                           std::string & output)
{
  _LOG_INFO("begin stringSplitB");

  char delim = ':';

  std::stringstream ss;
  ss.str(input);
  std::string item;

  std::vector<std::string> array;
  array.clear();

  while (std::getline(ss, item, delim))
  {
    array.push_back(item);
  }

  for (unsigned int i = 0; i < array.size(); i++)
  {
    //cout << array[i] << endl;
    _LOG_INFO("array: " << array[i]);
  }

  output = array[1];

  _LOG_INFO("end stringSplitB");
  return 0;
}

int
ndn::CMyJSON::charOutOfHead(const std::string & input,
                            std::string & output)
{
  _LOG_INFO("begin charOutOfHead");

  std::string ipt(input);

  size_t posh = ipt.rfind("\"");
  if (posh == std::string::npos)
  {
    _LOG_INFO("charOutOfHead nothing found!");
    //throw new CMyJsonResultException{"charOutOfHead", __GLOBAL_RESULT_JSON_CHAR_OUT_OF_HEAD};
    return -1;
  }
  ipt.erase(posh, 1);

  output = ipt;

  _LOG_INFO("end charOutOfHead");
  return 0;
}

int
ndn::CMyJSON::charOutOfTail(const std::string & input,
                            std::string & output)
{
  _LOG_INFO("begin charOutOfTail");

  std::string ipt(input);

  size_t post = ipt.find("\"");
  if (post == std::string::npos)
  {
    _LOG_INFO("charOutOfTail nothing found!");

    //throw new CMyJsonResultException{"charOutOfTail", __GLOBAL_RESULT_JSON_CHAR_OUT_OF_TAIL};
    return -1;
  }
  ipt.erase(post, 1);

  output = ipt;

  _LOG_INFO("end charOutOfTail");
  return 0;
}

int
ndn::CMyJSON::charOutOfHeadTail(const std::string & input,
                                std::string & output)
{
  _LOG_INFO("begin charOutOfHeadTail");

  std::string ipt(input);

  size_t posh = ipt.rfind("\"");
  if (posh == std::string::npos)
  {
    _LOG_INFO("charOutOfHead nothing found!");

    //throw new CMyJsonResultException{"charOutOfHeadTail", __GLOBAL_RESULT_JSON_CHAR_OUT_OF_HEADTAIL};
    return -1;
  }
  ipt.erase(posh, 1);

  size_t post = ipt.find("\"");
  if (post == std::string::npos)
  {
    _LOG_INFO("charOutOfTail nothing found!");
    //throw new CMyJsonResultException{"charOutOfHeadTail", __GLOBAL_RESULT_JSON_CHAR_OUT_OF_HEADTAIL};
    return -1;
  }
  ipt.erase(post, 1);

  output = ipt;

  _LOG_INFO("end charOutOfHeadTail");
  return 0;
}

int
ndn::CMyJSON::stringSearthRemaining(const std::string & source,
                                    const std::string & search,
                                    std::string & remain)
{
  _LOG_INFO("begin stringSearthRemaining");

  //size_t pos = source.find(search, 0);
  size_t pos = source.find(search);
  if (pos == std::string::npos)
  {
    _LOG_INFO("stringSearthRemaining finished!");
    return -1;
  }
 //fprintf(stderr, "%ld\n", pos);
  _LOG_INFO("pos=" << pos);

  size_t lth = search.length();
 //fprintf(stderr, "%ld\n", lth);
  _LOG_INFO("lth=" << lth);

  /*if (pos == (size_t)-1)
  {
   //fprintf(stderr, "noting found!\n");
    _LOG_INFO("noting found!");
    return -1;
  }*/

  /*if (lth == (size_t)-1)
  {
   //fprintf(stderr, "searching word empty!\n");
    _LOG_INFO("searching word empty!");
    return -1;
  }*/

//  std::string substr = source.substr(pos+lth+1);//不带/
  std::string substr = source.substr(pos+lth);//带/
 //fprintf(stderr, "%s\n", substr.c_str());
  _LOG_INFO("substr=" << substr);
  remain = substr;

  _LOG_INFO("end stringSearthRemaining");
  stringSearthRemaining(substr, search, remain);

  return 0;
}

/////////////////////////from here jsoncpp reconstruct
int
ndn::CMyJSON::writeJSON(const std::string & methodValue,
                        const std::string & paramsValue,
                        std::string & json)
{
  _LOG_INFO("begin writeJSON");

  if (methodValue.empty())
    throw new CMyJsonResultException{"writeJSON input empty", __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY};

  Json::Value root;
  Json::FastWriter fast;

  //method
  root["method"] = Json::Value(methodValue.c_str());

  //params
  root["params"].append(paramsValue.c_str());

  std::string result = fast.write(root);
  //cout << result << endl;
  _LOG_INFO("result=" << result);
  json = result;

  _LOG_INFO("end writeJSON");
  return 0;
}

int
ndn::CMyJSON::writeJSONArray(const std::string & methodValue,
                             const std::vector<std::string> & arrayValue,
                             std::string & json)
{
  _LOG_INFO("begin writeJSONArray");

  if (methodValue.empty() || arrayValue.empty())
    throw new CMyJsonResultException{"writeJSONArray input empty", __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY};

  Json::Value root;
  Json::FastWriter fast;

  //method
  root["method"] = Json::Value(methodValue.c_str());

  //params
  for (unsigned int i = 0; i < arrayValue.size(); i++)
  {
    //cout << arrayValue[i] << endl;
    root["params"].append(arrayValue[i]);
    _LOG_INFO("arrayValue: " << arrayValue[i]);
  }

  std::string result = fast.write(root);
  //cout << result << endl;
  _LOG_INFO("result=" << result);
  json = result;

  _LOG_INFO("end writeJSONArray");
  return 0;
}

int
ndn::CMyJSON::writeJSONArraySendToAddress(const std::string & methodValue,
                                          const std::vector<std::string> & arrayValue,
                                          std::string & json)
{
  _LOG_INFO("begin writeJSONArraySendToAddress");

  if (methodValue.empty() || arrayValue.empty())
    throw new CMyJsonResultException{"writeJSONArraySendToAddress input empty", __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY};

  Json::Value root;
  Json::FastWriter fast;

  //method
  root["method"] = Json::Value(methodValue.c_str());

  //params
  //cout << arrayValue[0] << endl;
  root["params"].append(arrayValue[0]);
  //cout << arrayValue[1] << endl;
  root["params"].append(atof(arrayValue[1].c_str()));
  //cout << arrayValue[2] << endl;
  root["params"].append(arrayValue[2]);

  std::string result = fast.write(root);
  //cout << result << endl;
  _LOG_INFO("result=" << result);
  json = result;

  _LOG_INFO("end writeJSONArraySendToAddress");
  return 0;
}

int
ndn::CMyJSON::writeJSONArraySendBnRawTransaction(const std::string & methodValue,
                                                 const std::vector<std::string> & arrayValue,
                                                 std::string & json)
{
  _LOG_INFO("begin writeJSONArraySendBnRawTransaction");

  if (methodValue.empty() || arrayValue.empty())
    throw new CMyJsonResultException{"writeJSONArraySendBnRawTransaction input empty", __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY};

  /*Json::Value root;
  Json::Value inner;
  Json::FastWriter fast;

  //method
  root["method"] = Json::Value(methodValue.c_str());

  //inner
  inner["reqseq"] = arrayValue[0];
  inner["addr"] = arrayValue[1];
  //double amt = atof(arrayValue[2].c_str());
  //amt = amt - __JSONCPP_PRECISION_DEBUG;
  //inner["amount"] = amt;
  inner["amount"] = atof(arrayValue[2].c_str());
  inner["bnInfo"] = arrayValue[3];

  //params
  root["params"].append(inner);

  std::string result = fast.write(root);
  json = result;*/

  rapidjson::Document root;
  root.SetObject();
  rapidjson::Document::AllocatorType & allocator = root.GetAllocator();

  root.AddMember("method", rapidjson::Value(methodValue.c_str(), allocator), allocator);

  rapidjson::Value inner(rapidjson::kObjectType);
  inner.AddMember("reqseq", rapidjson::Value(arrayValue[0].c_str(), allocator), allocator);
  inner.AddMember("addr", rapidjson::Value(arrayValue[1].c_str(), allocator), allocator);
  inner.AddMember("amount", atof(arrayValue[2].c_str()), allocator);
  inner.AddMember("bnInfo", rapidjson::Value(arrayValue[3].c_str(), allocator), allocator);

  rapidjson::Value array(rapidjson::kArrayType);
  array.PushBack(inner, allocator);

  root.AddMember("params", array, allocator);

  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  root.Accept(writer);

  json = sb.GetString();

  _LOG_INFO("end writeJSONArraySendBnRawTransaction");
  return 0;
}

int
ndn::CMyJSON::writeJSONArrayGetBnRawTransaction(const std::string & methodValue,
                                                const std::vector<std::string> & arrayValue,
                                                std::string & json)
{
  _LOG_INFO("begin writeJSONArrayGetBnRawTransaction");

  if (methodValue.empty() || arrayValue.empty())
    throw new CMyJsonResultException{"writeJSONArrayGetBnRawTransaction input empty", __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY};

  Json::Value root;
  Json::FastWriter fast;

  //method
  root["method"] = Json::Value(methodValue.c_str());

  //params
  //cout << arrayValue[0] << endl;
  root["params"].append(arrayValue[0]);

  std::string result = fast.write(root);
  //cout << result << endl;
  _LOG_INFO("result=" << result);
  json = result;

  _LOG_INFO("end writeJSONArrayGetBnRawTransaction");
  return 0;
}

int
ndn::CMyJSON::writeJSONGetBalance(const std::string & methodValue,
                                  const std::string & paramsValue,
                                  std::string & json)
{
  _LOG_INFO("begin writeJSONGetBalance");

  if (methodValue.empty() || paramsValue.empty())
    throw new CMyJsonResultException{"writeJSONGetBalance input empty", __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY};

  Json::Value root;
  Json::FastWriter fast;

  //method
  root["method"] = Json::Value(methodValue.c_str());

  //params
  root["params"].resize(0);

  std::string result = fast.write(root);
  //cout << result << endl;
  _LOG_INFO("result=" << result);
  json = result;

  _LOG_INFO("end writeJSONGetBalance");
  return 0;
}

int ndn::CMyJSON::writeJSONS3File(const string & user,
		  	  	  	                  const string & host,
		  	  	  	                  const string & access,
                                  const string & secret,
                                  const string & bucket,
                                  const string & key,
                                  string & json)
{
	_LOG_INFO("begin writeJSONPutFile");

  if (user.empty() || host.empty() || access.empty() || secret.empty() || bucket.empty() || key.empty())
    throw new CMyJsonResultException{"writeJSONS3File input empty", __GLOBAL_RESULT_JSON_FUNCTION_PRECONDITION_EMPTY};

	  Json::Value root;
	  Json::FastWriter fast;

	  root["S3Host"] = Json::Value(host.c_str());
	  root["S3User"] = Json::Value(user.c_str());
	  root["S3Access"] = Json::Value(access.c_str());
	  root["S3Secret"] = Json::Value(secret.c_str());
	  root["S3Bucket"] = Json::Value(bucket.c_str());
	  root["S3DstFile"] = Json::Value(key.c_str());

	  std::string result = fast.write(root);

	  _LOG_INFO("result=" << result);
	  json = result;

	  _LOG_INFO("end writeJSONPutFile");
	return 0;
}

int
ndn::CMyJSON::readJSON(const std::string & input,
                       std::string & result,
                       std::string & error,
                       std::string & id)
{
  _LOG_INFO("begin readJSON");

  Json::Reader reader;
  Json::Value value;

  if (!reader.parse(input, value))
    throw new CMyJsonResultException{"readJSON err", __GLOBAL_RESULT_JSON_JSONCPP_READER_PARSE_ERR};

  if (value["result"].isNull())
    throw new CMyJsonResultException{"readJSON err", __GLOBAL_RESULT_JSON_JSONCPP_READ_VALUE_EMPTY};

  if (reader.parse(input, value))
  {
    if (!value["result"].isNull())
    {
      for (unsigned int i = 0; i < value["result"].size(); i++)
      {
        string item = value["result"][i].asString();
        result = item;
        //cout << item << endl;
        _LOG_INFO("item=" << item);
      }
      //cout << value["error"].asString() << endl;
      error = value["error"].asString();
      //cout << value["id"].asString() << endl;
      id = value["id"].asString();
    }
  }

  _LOG_INFO("end readJSON");
  return 0;
}

int
ndn::CMyJSON::readJSONSendToAddress(const std::string & input,
                                    std::string & result,
                                    std::string & error,
                                    std::string & id)
{
  _LOG_INFO("begin readJSONSendToAddress");

  Json::Reader reader;
  Json::Value value;

  if (!reader.parse(input, value))
    throw new CMyJsonResultException{"readJSONSendToAddress err", __GLOBAL_RESULT_JSON_JSONCPP_READER_PARSE_ERR};

  if (value["result"].isNull())
    throw new CMyJsonResultException{"readJSONSendToAddress err", __GLOBAL_RESULT_JSON_JSONCPP_READ_VALUE_EMPTY};

  if (reader.parse(input, value))
  {
    if (!value["result"].isNull())
    {
      result = value["result"].asString();
      //cout << value["error"].asString() << endl;
      error = value["error"].asString();
      //cout << value["id"].asString() << endl;
      id = value["id"].asString();
    }
  }

  _LOG_INFO("end readJSONSendToAddress");
  return 0;
}

int
ndn::CMyJSON::readJSONSendBnRawTransaction(const std::string & input,
                                           std::string & result,
                                           std::string & txid,
                                           std::string & reqseq,
                                           std::string & error,
                                           std::string & id)
{
  _LOG_INFO("begin readJSONSendBnRawTransaction");

  Json::Reader reader;
  Json::Value value;

  if (!reader.parse(input, value))
    throw new CMyJsonResultException{"readJSONSendBnRawTransaction err", __GLOBAL_RESULT_JSON_JSONCPP_READER_PARSE_ERR};

  if (value["result"].isNull())
    throw new CMyJsonResultException{"readJSONSendBnRawTransaction err", __GLOBAL_RESULT_JSON_JSONCPP_READ_VALUE_EMPTY};

  if (reader.parse(input, value))
  {
    if (!value["result"].isNull())
    {
      result = value["result"].toStyledString();
      txid = value["result"]["txid"].asString();
      reqseq = value["result"]["reqseq"].asString();
      error = value["error"].asString();
      id = value["id"].asString();
    }
  }

  _LOG_INFO("end readJSONSendBnRawTransaction");
  return 0;
}

int
ndn::CMyJSON::readJSONGetBnRawTransaction(const std::string & input,
                                          std::string & result,
                                          std::string & error,
                                          std::string & id)
{
  _LOG_INFO("begin readJSONGetBnRawTransaction");

  Json::Reader reader;
  Json::Value value;

  if (!reader.parse(input, value))
    throw new CMyJsonResultException{"readJSONGetBnRawTransaction err", __GLOBAL_RESULT_JSON_JSONCPP_READER_PARSE_ERR};

  if (value["result"].isNull())
    throw new CMyJsonResultException{"readJSONGetBnRawTransaction err", __GLOBAL_RESULT_JSON_JSONCPP_READ_VALUE_EMPTY};

  if (reader.parse(input, value))
  {
    if (!value["result"].isNull())
    {
      result.append("txid:");
      std::string str1 = value["result"]["txid"].asString();
      if(str1.empty())
      {
        BN_LOG_ERROR("txid is empty");
        return -1;
      }
      result.append(str1);
      BN_LOG_DEBUG("txid:%s",str1.c_str());
      result.append(",");
      result.append("bnInfo:");
      str1 = value["result"]["bnInfo"].asString();
      if(str1.empty())
      {
        BN_LOG_ERROR("bnInfo is empty");
        return -1;
      }
      result.append(str1);
      //cout << value["error"].asString() << endl;
      error = value["error"].asString();
      //cout << value["id"].asString() << endl;
      id = value["id"].asString();
    }
  }

  _LOG_INFO("end readJSONGetBnRawTransaction");
  return 0;
}

int
ndn::CMyJSON::readJSONGetBalance(const std::string & input,
                                 std::string & result,
                                 std::string & error,
                                 std::string & id)
{
  _LOG_INFO("begin readJSONGetBalance");

  Json::Reader reader;
  Json::Value value;

  if (!reader.parse(input, value))
    throw new CMyJsonResultException{"readJSONGetBalance err", __GLOBAL_RESULT_JSON_JSONCPP_READER_PARSE_ERR};

  if (value["result"].isNull())
    throw new CMyJsonResultException{"readJSONGetBalance err", __GLOBAL_RESULT_JSON_JSONCPP_READ_VALUE_EMPTY};

  if (reader.parse(input, value))
  {
    if (!value["result"].isNull())
    {
      //note
      char szBuf[32] = {0};
      sprintf(szBuf, "%.17f", value["result"].asDouble());
      //cout << szBuf << endl;
      result.append(szBuf);
      //cout << value["error"].asString() << endl;
      error = value["error"].asString();
      //cout << value["id"].asString() << endl;
      id = value["id"].asString();
    }
  }

  _LOG_INFO("end readJSONGetBalance");
  return 0;
}
