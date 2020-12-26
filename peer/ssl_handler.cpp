#include "ssl_handler.h"
#include "logger.h"
#include "base64.h"

#include <cstring>
#include <iostream>
#include <algorithm>

using namespace std;

SslHandler::SslHandler()
{
  // create RSA 4096 keypair
  EVP_PKEY_CTX *keyCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
  EVP_PKEY_keygen_init(keyCtx);
  EVP_PKEY_CTX_set_rsa_keygen_bits(keyCtx, 4096);
  EVP_PKEY *key = NULL;
  EVP_PKEY_keygen(keyCtx, &key);
  EVP_PKEY_CTX_free(keyCtx);

  // extract private key
  BIO *privateKeyBio = BIO_new(BIO_s_mem());
  PEM_write_bio_PrivateKey(privateKeyBio, key, NULL, NULL, 0, 0, NULL);
  int privateKeyLength = BIO_pending(privateKeyBio);
  privateKey = new unsigned char[privateKeyLength + 1];
  memset(privateKey, '\0', privateKeyLength);
  BIO_read(privateKeyBio, privateKey, privateKeyLength);

  // extract public key
  BIO *publicKeyBio = BIO_new(BIO_s_mem());
  PEM_write_bio_PUBKEY(publicKeyBio, key);
  int publicKeyLength = BIO_pending(publicKeyBio);
  publicKey = new unsigned char[publicKeyLength + 1];
  memset(publicKey, '\0', publicKeyLength);
  BIO_read(publicKeyBio, publicKey, publicKeyLength);

  info("Key generation successful\n");
}

string SslHandler::encryptMessage(string message)
{
  BIO *bio = BIO_new_mem_buf((void *)publicKey, -1);
  RSA *publicRsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);

  int rsaLengthLimit = RSA_size(publicRsa);
  unsigned char *messageArr = new unsigned char[message.length()];
  copy(message.begin(), message.end(), messageArr);

  unsigned char *encryptMsg = (unsigned char *)malloc(rsaLengthLimit);
  memset(encryptMsg, 0, rsaLengthLimit);

  int len = rsaLengthLimit - 11;

  RSA_public_encrypt(len, (unsigned char *)message.c_str(), encryptMsg, publicRsa, RSA_PKCS1_PADDING);

  BIO_free_all(bio);
  RSA_free(publicRsa);
  return encodeBase64(encryptMsg, rsaLengthLimit);
}

string SslHandler::decryptMessage(string encryptedMessageEncoded)
{
  string encryptedMessage = decodeBase64(encryptedMessageEncoded);
  unsigned char *encryptedArr = new unsigned char[encryptedMessage.length()];
  strcpy((char *)encryptedArr, encryptedMessage.c_str());
  copy(encryptedMessage.begin(), encryptedMessage.end(), encryptedArr);

  BIO *bio = BIO_new_mem_buf((void *)privateKey, -1);
  RSA *privateRsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
  int rsaLengthLimit = RSA_size(privateRsa);
  unsigned char *decryptMsg = new unsigned char[rsaLengthLimit];

  memset(decryptMsg, '\0', rsaLengthLimit);
  BIO_free_all(bio);
  RSA_private_decrypt(rsaLengthLimit, encryptedArr, decryptMsg, privateRsa, RSA_PKCS1_PADDING);
  RSA_free(privateRsa);
  string decryptMsgString((char *)decryptMsg);
  return decryptMsgString;
}