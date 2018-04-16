
#include "myinstqueue.hpp"
//begin 20170629 liulijin add
#include "../logging4.h"
#include "../openssl/myopenssl.hpp"
#include "../constant/myconstant.hpp"
//end 20170629 liulijin add

INIT_LOGGER("CMyInstQueue");

ndn::CMyInstQueue * ndn::CMyInstQueue::instance = new CMyInstQueue;

ndn::CMyInstQueue *
ndn::CMyInstQueue::getInstance()
{
  return instance;
}

int
ndn::CMyInstQueue::qGetSize(int & size)
{
  std::unique_lock<std::mutex> ul(mtx);
  size = qInst.size();
  ul.unlock();
  return 0;
}

int
ndn::CMyInstQueue::qSetSize(int size)
{
  q_size = size;
  return 0;
}

int
ndn::CMyInstQueue::qPush(void * inst)
{
  std::unique_lock<std::mutex> ul(mtx);
  qInst.push(inst);
  ul.unlock();
  return 0;
}

void *
ndn::CMyInstQueue::qPop()
{
  std::unique_lock<std::mutex> ul(mtx);
  if (qInst.empty())
  {
    ul.unlock();
   //fprintf(stderr, "queue is empty!\n");
    _LOG_INFO("queue is empty!");
    return NULL;
  }

  void * inst = qInst.front();
  qInst.pop();
  ul.unlock();
  return inst;
}

int
ndn::CMyInstQueue::qClear()
{
  std::unique_lock<std::mutex> ul(mtx);
  int size = 0;
  qGetSize(size);
  for (int i = 0; i < size; i++)
  {
    qInst.pop();
  }
  ul.unlock();
  return 0;
}
