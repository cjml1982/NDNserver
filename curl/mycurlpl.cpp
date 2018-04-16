
#include "mycurlpl.hpp"
#include "../mysql/myinstqueue.hpp"
//begin 20170601 liulijin add
#include "../logging4.h"
#include "../openssl/myopenssl.hpp"
//end 20170601 liulijin add

INIT_LOGGER("CMyCURLPL");

ndn::CMyCURLPL * ndn::CMyCURLPL::instance = new CMyCURLPL;

ndn::CMyCURLPL *
ndn::CMyCURLPL::getInstance()
{
  return instance;
}

size_t
ndn::CMyCURLPL::curlResponseCallback(char * data, size_t size, size_t nmemb, void * ptr)
{
  _LOG_INFO("begin curlResponseCallback");

  char * response = data;
  char * writeData = (char *)ptr;
  //fprintf(stderr, "response=%s, writeData=%s\n", response, writeData);
  _LOG_INFO("response=" << response << ",writeData=" << writeData);
  //note
  ndn::CMyCURLPL::getInstance()->curlRet = response;
  size_t sizeBlob = size * nmemb;
  //fprintf(stderr, "size=%ld, nmemb=%ld, sizeBlob=%ld\n", size, nmemb, sizeBlob);
  _LOG_INFO("size=" << size << ",nmemb=" << nmemb << ",sizeBlob=" << sizeBlob);

  _LOG_INFO("end curlResponseCallback");
  return sizeBlob;
}

int
ndn::CMyCURLPL::curlInit()
{
  BN_LOG_INTERFACE("begin curlInit");

  CURLcode rCode = curl_global_init(CURL_GLOBAL_ALL);
  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_global_init failed: %d \n", rCode);
    BN_LOG_ERROR("curl_global_init failed:%d ",rCode);
    //fprintf(stderr, "curl_global_init failed info: %s \n", curl_easy_strerror(rCode));
  
    return -1;
  }

  _LOG_INFO("end curlInit");
  return 0;
}

CURL *
ndn::CMyCURLPL::curlInstance()
{
  _LOG_INFO("begin curlInstance");

  CURL * curl = curl_easy_init();
  if (NULL == curl)
  {
    //fprintf(stderr, "curl_easy_init failed\n");
    BN_LOG_ERROR("curl_easy_init failed");
    return NULL;
  }

  _LOG_INFO("end curlInstance");
  return curl;
}

int
ndn::CMyCURLPL::curlBefore(CQIHolder * holder)
{
  _LOG_INFO("begin curlBefore");

  curl_easy_reset(holder->curl);
  _LOG_INFO("curl_easy_reset");

  _LOG_INFO("end curlBefore");
  return 0;
}

int
ndn::CMyCURLPL::curlPost(CQIHolder * holder, const char * url, const char * postheader, const char * postdata)
{
  _LOG_INFO("begin curlPost");

  //curl_easy_setopt(holder->curl, CURLOPT_TIMEOUT, 1);
  //curl_easy_setopt(holder->curl, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(holder->curl, CURLOPT_TIMEOUT_MS, 60*1000);
  curl_easy_setopt(holder->curl, CURLOPT_TCP_KEEPALIVE, 0L);
  curl_easy_setopt(holder->curl, CURLOPT_FORBID_REUSE, 1);
  curl_easy_setopt(holder->curl, CURLOPT_NOSIGNAL, 1);
  CURLcode rCode = curl_easy_setopt(holder->curl, CURLOPT_URL, url);
  if (CURLE_OK != rCode)
  {
    
    //fprintf(stderr, "curl_easy_setopt CURLOPT_URL failed: %d \n", rCode);
    BN_LOG_ERROR("curl_easy_setopt CURLOPT_URL failed: %d",rCode);
    //fprintf(stderr, "curl_easy_setopt CURLOPT_URL failed info: %s \n", curl_easy_strerror(rCode));
    BN_LOG_ERROR("curl_easy_setopt CURLOPT_URL failed info: %s", curl_easy_strerror(rCode));
    //ndn::CMyInstQueue::getInstance()->qPush(holder);
    return -1;
  }

  //-v
  //curl_easy_setopt(holder->curl, CURLOPT_VERBOSE, 1L);

  //-H
  //begin alter by liangyuanting
  holder->list = curl_slist_append(holder->list, postheader);
  rCode = curl_easy_setopt(holder->curl, CURLOPT_HTTPHEADER, holder->list);
  //end alter by liangyuanting

  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_easy_setopt CURLOPT_HTTPHEADER failed: %d \n", rCode);
    BN_LOG_ERROR("curl_easy_setopt CURLOPT_HTTPHEADER failed: %d",rCode);
    //fprintf(stderr, "curl_easy_setopt CURLOPT_HTTPHEADER failed info: %s \n", curl_easy_strerror(rCode));
    BN_LOG_ERROR("curl_easy_setopt CURLOPT_HTTPHEADER failed info: %s", curl_easy_strerror(rCode));
    //ndn::CMyInstQueue::getInstance()->qPush(holder);
    return -1;
  }

  //-d
  curl_easy_setopt(holder->curl, CURLOPT_POST, 1L);
  curl_easy_setopt(holder->curl, CURLOPT_POSTFIELDSIZE, strlen(postdata));
  rCode = curl_easy_setopt(holder->curl, CURLOPT_POSTFIELDS, postdata);
  if (CURLE_OK != rCode)
  {
    
    BN_LOG_ERROR("curl_easy_setopt failed: %d",rCode);
    //fprintf(stderr, "curl_easy_perform failed info: %s \n", curl_easy_strerror(rCode));
    BN_LOG_ERROR("curl_easy_setopt failed info:%s ", curl_easy_strerror(rCode));
    
    return -1;
  }

  curl_easy_setopt(holder->curl, CURLOPT_WRITEFUNCTION, curlResponseCallback);
  curl_easy_setopt(holder->curl, CURLOPT_WRITEDATA, "final pointer");

  _LOG_INFO("end curlPost");
  return 0;
}

int
ndn::CMyCURLPL::curlGet(CQIHolder * holder, const char * url)
{
  _LOG_INFO("begin curlGet");

  curl_easy_setopt(holder->curl, CURLOPT_TIMEOUT, 10);
  curl_easy_setopt(holder->curl, CURLOPT_URL, url);

  curl_easy_setopt(holder->curl, CURLOPT_HTTPGET, 1);
  _LOG_INFO("curl_easy_setopt CURLOPT_HTTPGET");

  _LOG_INFO("end curlGet");
  return 0;
}

int
ndn::CMyCURLPL::curlAfter(CQIHolder * holder)
{
  _LOG_INFO("begin curlAfter");

  CURLcode rCode = curl_easy_perform(holder->curl);
  if (CURLE_OK != rCode)
  {
    //fprintf(stderr, "curl_easy_perform failed: %d \n", rCode);
    BN_LOG_ERROR("curl_easy_perform failed: %d",rCode);
    //fprintf(stderr, "curl_easy_perform failed info: %s \n", curl_easy_strerror(rCode));
    BN_LOG_ERROR("curl_easy_perform failed info:%s ", curl_easy_strerror(rCode));
    return -1;
  }

  _LOG_INFO("end curlAfter");
  return 0;
}

int
ndn::CMyCURLPL::curlResponseCode(CQIHolder * holder, int & output)
{
  _LOG_INFO("begin curlResponseCode");

  int responseCode = -1;
  CURLcode rCode = curl_easy_getinfo(holder->curl, CURLINFO_RESPONSE_CODE, &responseCode);
  if (CURLE_OK != rCode)
  {
   //fprintf(stderr, "curl_easy_getinfo failed: %d \n", rCode);
    BN_LOG_ERROR("curl_easy_getinfo failed: %d",rCode);
    //fprintf(stderr, "curl_easy_perform failed info: %s \n", curl_easy_strerror(rCode));
    BN_LOG_ERROR("curl_easy_getinfo failed info:%s ", curl_easy_strerror(rCode));
   
    return -1;
  }

  output = responseCode;

  _LOG_INFO("end curlResponseCode");
  return 0;
}

int
ndn::CMyCURLPL::curlResponse(CQIHolder * holder, std::string & output)
{

  output = curlRet;
  //note: it must be empty after used
  curlRet = "";

  return 0;
}

//begin alter by liangyuanting
int
ndn::CMyCURLPL::doHTTPPost(CQIHolder * holder,
                           std::string & sURL,
                           std::string & sHeader,
                           std::string & sBody,
                           int & responseCode,
                           std::string & response)
{
  _LOG_INFO("doHTTPPost starting...");
  int ret = 0;
  std::unique_lock<std::mutex> ul(mtx);

  curlBefore(holder);
  ret = curlPost(holder, sURL.c_str(), sHeader.c_str(), sBody.c_str());
  if (ret == -1)
  {
    throw new CMyCurlplResultException{"curlPost err", __GLOBAL_RESULT_CURL_POST_ERR};
  }
  ret = curlAfter(holder);
  if (ret == -1)
  {
    throw new CMyCurlplResultException{"curlAfter err", __GLOBAL_RESULT_CURL_PERFORM_ERR};
  }
  int reCode = -1;
  ret = curlResponseCode(holder, reCode);
  if (ret == -1)
  {
    throw new CMyCurlplResultException{"curlResponseCode err", __GLOBAL_RESULT_CURL_RESPONSE_ERR};
  }
  responseCode = reCode;
  std::string res("");
  curlResponse(holder, res);
  response = res;

  if (NULL != holder->list)
  {
    curl_slist_free_all(holder->list);
    holder->list = NULL;
    BN_LOG_DEBUG("curl_slist_free_all list had been free normally!");
  }

//  ul.unlock();

  return 0;
}

//end alter by liangyuanting
