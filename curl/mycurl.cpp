
#include "mycurl.hpp"
//begin 20170601 liulijin add
#include "../logging4.h"
#include "../openssl/myopenssl.hpp"
//end 20170601 liulijin add

INIT_LOGGER("CMyCURL");

ndn::CMyCURL * ndn::CMyCURL::instance = new CMyCURL;

ndn::CMyCURL *
ndn::CMyCURL::getInstance()
{
  return instance;
}

size_t
ndn::CMyCURL::curlResponseCallback(char * data, size_t size, size_t nmemb, void * ptr)
{
  _LOG_INFO("begin curlResponseCallback");

  char * response = data;
  char * writeData = (char *)ptr;
  //fprintf(stderr, "response=%s, writeData=%s\n", response, writeData);
  _LOG_INFO("response=" << response << ",writeData=" << writeData);
  //note
  ndn::CMyCURL::getInstance()->curlRet = response;
  size_t sizeBlob = size * nmemb;
  //fprintf(stderr, "size=%ld, nmemb=%ld, sizeBlob=%ld\n", size, nmemb, sizeBlob);
  _LOG_INFO("size=" << size << ",nmemb=" << nmemb << ",sizeBlob=" << sizeBlob);

  _LOG_INFO("end curlResponseCallback");
  return sizeBlob;
}

int
ndn::CMyCURL::curlInit()
{
  _LOG_INFO("begin curlInit");

  CURLcode rCode = curl_global_init(CURL_GLOBAL_ALL);
  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_global_init failed: %d \n", rCode);
    _LOG_INFO("curl_global_init failed: " << rCode);
    //fprintf(stderr, "curl_global_init failed info: %s \n", curl_easy_strerror(rCode));
    _LOG_INFO("curl_global_init failed info: " << curl_easy_strerror(rCode));
    return -1;
  }

  _LOG_INFO("end curlInit");
  return 0;
}

int
ndn::CMyCURL::curlInstance()
{
  _LOG_INFO("begin curlInstance");

  curl = curl_easy_init();
  if (NULL == curl )
  {
    //fprintf(stderr, "curl_easy_init failed\n");
    _LOG_INFO("curl_easy_init failed");
    return -1;
  }

  _LOG_INFO("end curlInstance");
  return 0;
}

int
ndn::CMyCURL::curlBefore()
{
  _LOG_INFO("begin curlBefore");

  curl_easy_reset(curl);
  _LOG_INFO("curl_easy_reset");

  _LOG_INFO("end curlBefore");
  return 0;
}

int
ndn::CMyCURL::curlPost(const char * url, const char * postheader, const char * postdata)
{
  _LOG_INFO("begin curlPost");

  //curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
  //curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 60*1000);
  curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 0L);
  curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
  //curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
  CURLcode rCode = curl_easy_setopt(curl, CURLOPT_URL, url);
  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_easy_setopt CURLOPT_URL failed: %d \n", rCode);
    _LOG_INFO("curl_easy_setopt CURLOPT_URL failed: " << rCode);
    //fprintf(stderr, "curl_easy_setopt CURLOPT_URL failed info: %s \n", curl_easy_strerror(rCode));
    _LOG_INFO("curl_easy_setopt CURLOPT_URL failed info: " << curl_easy_strerror(rCode));
    return -1;
  }

  //-v
  //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  //-H
  list = curl_slist_append(list, postheader);
  rCode = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_easy_setopt CURLOPT_HTTPHEADER failed: %d \n", rCode);
    _LOG_INFO("curl_easy_setopt CURLOPT_HTTPHEADER failed: " << rCode);
    //fprintf(stderr, "curl_easy_setopt CURLOPT_HTTPHEADER failed info: %s \n", curl_easy_strerror(rCode));
    _LOG_INFO("curl_easy_setopt CURLOPT_HTTPHEADER failed info: " << curl_easy_strerror(rCode));
    return -1;
  }

  //-d
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(postdata));
  rCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata);
  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_easy_setopt CURLOPT_POSTFIELDS failed: %d \n", rCode);
    _LOG_INFO("curl_easy_setopt CURLOPT_POSTFIELDS failed: " << rCode);
    //fprintf(stderr, "curl_easy_setopt CURLOPT_POSTFIELDS failed info: %s \n", curl_easy_strerror(rCode));
    _LOG_INFO("curl_easy_setopt CURLOPT_POSTFIELDS failed info: " << curl_easy_strerror(rCode));
    return -1;
  }

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlResponseCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, "final pointer");

  _LOG_INFO("end curlPost");
  return 0;
}

int
ndn::CMyCURL::curlGet(const char * url)
{
  _LOG_INFO("begin curlGet");

  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(curl, CURLOPT_URL, url);

  curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
  _LOG_INFO("curl_easy_setopt CURLOPT_HTTPGET");

  _LOG_INFO("end curlGet");
  return 0;
}

int
ndn::CMyCURL::curlAfter()
{
  _LOG_INFO("begin curlAfter");

  CURLcode rCode = curl_easy_perform(curl);
  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_easy_perform failed: %d \n", rCode);
    BN_LOG_ERROR("curl_easy_perform failed: %d" ,rCode);
    //fprintf(stderr, "curl_easy_perform failed info: %s \n", curl_easy_strerror(rCode));
    BN_LOG_ERROR("curl_easy_perform failed info: %s", curl_easy_strerror(rCode));
    return -1;
  }

  _LOG_INFO("end curlAfter");
  return 0;
}

int
ndn::CMyCURL::curlResponseCode(int & output)
{
  _LOG_INFO("begin curlResponseCode");

  int responseCode = -1;
  CURLcode rCode = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
  if (CURLE_OK != rCode)
  {
   //fprintf(stderr, "curl_easy_getinfo failed: %d \n", rCode);
    _LOG_INFO("curl_easy_getinfo failed: " << rCode);
   //fprintf(stderr, "curl_easy_getinfo failed info: %s \n", curl_easy_strerror(rCode));
    _LOG_INFO("curl_easy_getinfo failed info: " << curl_easy_strerror(rCode));
    return -1;
  }
 //fprintf(stderr, "responseCode=%d \n", responseCode);
  _LOG_INFO("responseCode=" << responseCode);
  output = responseCode;

  _LOG_INFO("end curlResponseCode");
  return 0;
}

int
ndn::CMyCURL::curlResponse(std::string & output)
{
  _LOG_INFO("begin curlResponse");

  _LOG_INFO("curlRet=" << curlRet);
  output = curlRet;

  //note: it must be empty after used
  curlRet = "";

  _LOG_INFO("end curlResponse");
  return 0;
}

int
ndn::CMyCURL::curlDestroy()
{
  _LOG_INFO("begin curlDestroy");

  //if (NULL != list)
  //  curl_slist_free_all(list);
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  _LOG_INFO("curl_easy_cleanup curl_global_cleanup");

  _LOG_INFO("end curlDestroy");
  return 0;
}

int
ndn::CMyCURL::doHTTPPost(std::string & sURL,
                         std::string & sHeader,
                         std::string & sBody,
                         int & responseCode,
                         std::string & response)
{
  _LOG_INFO("doHTTPPost starting...");

  int ret = 0;
  ret = curlInit();
  if (ret == -1)
  {
    throw new CMyCurlResultException{"curlInit err", __GLOBAL_RESULT_CURL_INIT_ERR};
  }
  ret = curlInstance();
  if (ret == -1)
  {
    throw new CMyCurlResultException{"curlInstance err", __GLOBAL_RESULT_CURL_INST_ERR};
  }
  curlBefore();
  ret = curlPost(sURL.c_str(), sHeader.c_str(), sBody.c_str());
  if (ret == -1)
  {
    throw new CMyCurlResultException{"curlPost err", __GLOBAL_RESULT_CURL_POST_ERR};
  }
  ret = curlAfter();
  if (ret == -1)
  {
    throw new CMyCurlResultException{"curlAfter err ", __GLOBAL_RESULT_CURL_PERFORM_ERR};
  }
  int reCode = -1;
  ret = curlResponseCode(reCode);
  if (ret == -1)
  {
    throw new CMyCurlResultException{"curlResponseCode err ", __GLOBAL_RESULT_CURL_RESPONSE_ERR};
  }
  responseCode = reCode;
  std::string res("");
  curlResponse(res);
  response = res;

  curlDestroy();

  _LOG_INFO("doHTTPPost finished.");
  return ret;
}
