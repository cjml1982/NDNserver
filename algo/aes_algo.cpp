
#include <stdlib.h>  
#include <stdio.h>	
#include "aes_algo.h"  
//#include <openssl/evp.h>  

//begin 20170601 liulijin add
#include "../logging4.h"
//end 20170601 liulijin add
//michael 20180311 added

void handleErrors(void)  
{  
  ERR_print_errors_fp(stderr);	
  abort();	
}  

void hexdump(
                FILE *f,
                const char *title,
                const unsigned char *s,
                int l)
{
    int n = 0;

    //fprintf(f, "%s", title);
    //_LOG_INFO("title=" << title);
    for (; n < l; ++n) {
        if ((n % 16) == 0) {
                //fprintf(f, "\n%04x", n);
                //_LOG_INFO("n=" << n);
        }
        //fprintf(f, " %02x", s[n]);
        //_LOG_INFO("s[n]=" << s[n]);
    }

    //fprintf(f, "\n");
    //_LOG_INFO("");
}

 
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,  
  unsigned char *iv, unsigned char *ciphertext)  
{  
  EVP_CIPHER_CTX *ctx;	
  
  int len;	
  
  int ciphertext_len;  
  
  /* Create and initialise the context */  
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();  
  
  /* Initialise the encryption operation. IMPORTANT - ensure you use a key 
   * and IV size appropriate for your cipher 
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The 
   * IV size for *most* modes is the same as the block size. For AES this 
   * is 128 bits */  
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))  
	handleErrors();  
  
  /* Provide the message to be encrypted, and obtain the encrypted output. 
   * EVP_EncryptUpdate can be called multiple times if necessary 
   */  
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))  
	handleErrors();  
  ciphertext_len = len;  
  
  /* Finalise the encryption. Further ciphertext bytes may be written at 
   * this stage. 
   */  
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();  
  ciphertext_len += len;  
  
  /* Clean up */  
  EVP_CIPHER_CTX_free(ctx);  
  
  return ciphertext_len;  
}  
  
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,	
  unsigned char *iv, unsigned char *plaintext)	
{  
  EVP_CIPHER_CTX *ctx;	
  
  int len;	
  
  int plaintext_len;  
  
  /* Create and initialise the context */  
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();  
  
  /* Initialise the decryption operation. IMPORTANT - ensure you use a key 
   * and IV size appropriate for your cipher 
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The 
   * IV size for *most* modes is the same as the block size. For AES this 
   * is 128 bits */  
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))  
	handleErrors();  
  
  /* Provide the message to be decrypted, and obtain the plaintext output. 
   * EVP_DecryptUpdate can be called multiple times if necessary 
   */  
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))	
	handleErrors();  
  plaintext_len = len;	
  
  /* Finalise the decryption. Further plaintext bytes may be written at 
   * this stage. 
   */  
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();	
  plaintext_len += len;  
  
  /* Clean up */  
  EVP_CIPHER_CTX_free(ctx);  
  
  return plaintext_len;  
}  

