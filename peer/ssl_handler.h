// ssl_handler.h
#ifndef SSL_HANDLER_H
#define SSL_HANDLER_H

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <string>

class SslHandler
{
public:
  SslHandler();
  std::string encryptMessage(std::string);
  std::string decryptMessage(std::string);

private:
  unsigned char * privateKey;
  unsigned char * publicKey;
};

#endif