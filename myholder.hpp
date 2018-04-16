
#ifndef CQIHOLDER_HPP
#define CQIHOLDER_HPP

#include <my_global.h>
#include <mysql.h>
#include <curl/curl.h>


#ifdef __cplusplus
extern "C" {
#endif

namespace ndn {

class CQIHolder
{
    public:

      CQIHolder()
      {
        curl = NULL;
        client = NULL;
        list = NULL;
      }
      ~CQIHolder()
      {
        curl = NULL;
        client = NULL;
        list = NULL;
      }



    public:
      MYSQL * client;
      CURL  * curl;
      //add the var
      curl_slist * list;
};

}

#ifdef __cplusplus
}
#endif

#endif
