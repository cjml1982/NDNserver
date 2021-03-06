
#ifndef ALGO_AES_H  
#define ALGO_AES_H  
#include <openssl/evp.h>
#include <openssl/err.h>

//michael 20180311 added
using namespace std;
#include <iostream>

#include "../constant/myresultcode.hpp"
class CAlgoResultException : public CMy_BSDR_Exception
{
public:
  CAlgoResultException(const std::string & info, int g_rescode)
    : CMy_BSDR_Exception{info, g_rescode}
  {
  }
  virtual ~CAlgoResultException() {};
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


#define MAX_TEXT_SIZE 1048576
#define KEY_BYTE_NUMBERS 16 //16 bytes are 128 bits

static unsigned char KEY[] = {
    0x68, 0xde, 0xca, 0xe8, 0x86, 0x06, 0xb2, 0x6b,
    0x9b, 0xb9, 0x46, 0x88, 0x54, 0xed, 0x7f, 0x31
};

static unsigned char INITIAL_VECTOR[] = {
0xcd, 0x3c, 0x13, 0x69, 0xa3, 0x93, 0x0a, 0x9e, 0x50, 0xde, 0xb1, 0xfe, 0x2e, 0x8e, 0xa6, 0xe4,
0xd4, 0x72, 0x53, 0xab, 0xe8, 0x54, 0xcb, 0x81, 0x38, 0x27, 0xc7, 0x15, 0xe0, 0xd9, 0xad, 0xe1,
0x8c, 0x71, 0x14, 0x8b, 0x1c, 0x96, 0x71, 0x8a, 0xa3, 0xed, 0x4c, 0xcd, 0xcd, 0xde, 0x79, 0x2a,
0x41, 0x89, 0x85, 0xa1, 0x58, 0x60, 0xe3, 0x1b, 0x59, 0x92, 0xf8, 0xe3, 0xba, 0x92, 0xee, 0x93
};

void handleErrors(void) ;
void hexdump(
                FILE *f,
                const char *title,
                const unsigned char *s,
                int l);

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,  
  unsigned char *iv, unsigned char *ciphertext);  
  
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,  
  unsigned char *iv, unsigned char *plaintext);  
  
#endif 

