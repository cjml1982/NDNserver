
#ifndef MYINSTQUEUE_HPP
#define MYINSTQUEUE_HPP

#include <iostream>
#include <queue>
#include <mutex>

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CMyIQResultException : public CMy_BSDR_Exception
{
public:
  CMyIQResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CMyIQResultException() {};
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

class CMyInstQueue {

public:
  CMyInstQueue() {}
  ~CMyInstQueue() {}

  static CMyInstQueue *
  getInstance();

public:

  int
  qGetSize(int & size);

  int
  qSetSize(int size);

public:

  int
  qPush(void * inst);

  void *
  qPop();

public:

  int
  qClear();

private:
  static CMyInstQueue * instance;

  std::mutex mtx;

  std::queue<void *> qInst;
  int q_size;
};

}

#ifdef __cplusplus
}
#endif

#endif
