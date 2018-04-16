
#include "myopenssl.hpp"

INIT_LOGGER("CMyOpenSSL");

ndn::CMyOpenSSL * ndn::CMyOpenSSL::instance = new CMyOpenSSL;

ndn::CMyOpenSSL *
ndn::CMyOpenSSL::getInstance()
{
  return instance;
}

int
ndn::CMyOpenSSL::SHA256(const std::string & input,
                        std::string & output)
{

  char buf[3];
  unsigned char hash[SHA256_DIGEST_LENGTH];

  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, input.c_str(), input.size());
  SHA256_Final(hash, &sha256);

  std::string result("");
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    sprintf(buf,"%02x",hash[i]);
    result = result + buf;
  }

  output = result;

  OPENSSL_cleanse(&sha256, sizeof(sha256));
  return 0;
}

/**
 ********* example **********
 * unsigned long t;
 * getTimeStamp(&t);
 * fprintf(stderr, "%ld", t);
 ********* example **********
 * unsigned long t;
 * ndn::CMyOpenSSL::getInstance()->getTimeStamp(&t);
 * fprintf(stderr, "%ld", t);
 ****************************
 * Get UTC
 * shell> date -d @1497587048
 * Get Unix time
 * shell> date -d @1497587048 +%s
 */
int
ndn::CMyOpenSSL::getTimeStamp(unsigned long * t)
{
  struct timeval tm;
  unsigned long ms;
  gettimeofday(&tm, NULL);
  //ms = tm.tv_sec;
  ms = tm.tv_sec * 1000 + tm.tv_usec / 1000;
  *t = ms;

  return 0;
}

/**
 ********* example 1 **********
 * char * s;
 * getTimeStamp2(&s);
 * fprintf(stderr, "%s", s);
 ********* example 2 **********
 * char * s;
 * ndn::CMyOpenSSL::getInstance()->getTimeStamp2(&s);
 * fprintf(stderr, "%s", s);
 */
int
ndn::CMyOpenSSL::getTimeStamp2(char ** t)
{
  char * s;
  char stime[256] = { 0 };
  time_t now_time;
  time(&now_time);
  strftime(stime, sizeof(stime), "%F %H:%M:%S", localtime(&now_time));
  s = stime;
  sprintf(*t, "%s", s);

  return 0;
}

int
ndn::CMyOpenSSL::printTimeNow()
{
  if (isTimeOff)
  {
    return -1;
  }

  unsigned long t;
  getTimeStamp(&t);
  fprintf(stderr, "==================================================================\n");
  fprintf(stderr, "=============/>> now time is: %ld (ms) \n", t);
  fprintf(stderr, "==================================================================\n");

  return 0;
}

int
ndn::CMyOpenSSL::printTimeBegin(unsigned long * t)
{
  if (isTimeOff)
  {
    return -1;
  }

  struct timeval tm;
  unsigned long ms;
  gettimeofday(&tm, NULL);
  //ms = tm.tv_sec;
  ms = tm.tv_sec * 1000 + tm.tv_usec / 1000;
  *t = ms;
  fprintf(stderr, "==================================================================\n");
  fprintf(stderr, "=============/>> begin time is: %ld (ms) \n", ms);
  fprintf(stderr, "==================================================================\n");

  return 0;
}

int
ndn::CMyOpenSSL::printTimeBeginV2(unsigned long * t)
{
  struct timeval tm;
  unsigned long ms;
  gettimeofday(&tm, NULL);
  //ms = tm.tv_sec;
  ms = tm.tv_sec * 1000 + tm.tv_usec / 1000;
  *t = ms;
  fprintf(stderr, "==================================================================\n");
  fprintf(stderr, "=============/>> begin time is: %ld (ms) \n", ms);
  fprintf(stderr, "==================================================================\n");

  return 0;
}

int
ndn::CMyOpenSSL::printTimeEnd(unsigned long lbegin)
{
  if (isTimeOff)
  {
    return -1;
  }

  unsigned long lend;
  getTimeStamp(&lend);

  fprintf(stderr, "==================================================================\n");
  fprintf(stderr, "=============/>> end time is: %ld (ms) \n", lend);
  fprintf(stderr, "==================================================================\n");
  float cost = (float)(lend - lbegin) / 1000;
  fprintf(stderr, "=============/>> cost time is: %f (s) \n", cost);
  fprintf(stderr, "==================================================================\n");

  return 0;
}

int
ndn::CMyOpenSSL::printTimeEndV2(unsigned long lbegin)
{
  unsigned long lend;
  getTimeStamp(&lend);

  fprintf(stderr, "==================================================================\n");
  fprintf(stderr, "=============/>> end time is: %ld (ms) \n", lend);
  fprintf(stderr, "==================================================================\n");
  float cost = (float)(lend - lbegin) / 1000;
  fprintf(stderr, "=============/>> cost time is: %f (s) \n", cost);
  fprintf(stderr, "==================================================================\n");

  return 0;
}

int
ndn::CMyOpenSSL::printTimeNowByFormatted()
{
  if (isTimeOff)
  {
    return -1;
  }

  unsigned long t;
  getTimeStamp(&t);

  char cmd[256] = {0};
  snprintf(cmd, sizeof(cmd), "date -d @%ld", t);
  system(cmd);

  return 0;
}

int
ndn::CMyOpenSSL::getFakeRandomUInt(const unsigned int istart,
                                   const unsigned int iend,
                                   unsigned int & iresult)
{
  struct timeval tm;
  unsigned long ms;
  gettimeofday(&tm, NULL);
  ms = tm.tv_sec * 1000 + tm.tv_usec / 1000;
  std::srand(ms);
  iresult = std::rand() % (iend - istart + 1) + istart;

  return 0;
}


int 
ndn::CMyOpenSSL::hash_string(std::string string_piece)
{
      size_t result = 0;                                                        
      for (auto it = string_piece.begin(); it != string_piece.end(); ++it) {  
              result = result + (*it)*7;                                        
      }
      _LOG_INFO("result:"<<result);
      return result;

}

int 
ndn::CMyOpenSSL::hash_string2(std::string string_piece)
{
      size_t result = 0;                                                        
      for (auto it = string_piece.begin(); it != string_piece.end(); ++it) {  
              result = result + (*it)*3;                                        
      }
      _LOG_INFO("result:"<<result);
      return result;

}

std::string
ndn::CMyOpenSSL::intToString(size_t ori)
{
    std::stringstream ss ;
    std::string result=NULL;
    ss<<ori;
    ss>>result;
    return result;
}
