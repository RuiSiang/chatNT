// ssl_handler.h
#ifndef SSL_HANDLER_H
#define SSL_HANDLER_H

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <string>

std::string sha256(std::string); //returns sha256 digest

//ssl class for handling crypto operations
class SslHandler
{
public:
  SslHandler(); //constructor
  std::string getHashId() { return hashId; } //get hash id
  std::string getPublicKey() { return publicKey; } //get public key

  std::string encryptMessage(std::string, std::string); //encrypts string and returns encrypted base64 string
  std::string decryptMessage(std::string); //decrypts encrypted base64 string
  std::string signMessage(std::string); //generate base64 digital signature of string
  bool verifyMessage(std::string, std::string, std::string); //validates base64 digital signature with original string

private:
  std::string hashId; //hash id of current user
  std::string publicKey; //public key of current user
  RSA *privateKey; //private key of current user
};

#endif