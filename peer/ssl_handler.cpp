#include "ssl_handler.h"
#include "logger.h"
#include "base64.h"

#include <cstring>
#include <sstream>
#include <vector>

#define keySize 4096

using namespace std;

//generates sha256 digest
string sha256(string str)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  char buffer[65];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, str.c_str(), str.length());
  SHA256_Final(hash, &sha256);
  int i = 0;
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
  {
    sprintf(buffer + (i * 2), "%02x", hash[i]);
  }
  buffer[64] = 0;
  return string(buffer);
}

//constructor
SslHandler::SslHandler()
{
  // create RSA 4096 keypair
  EVP_PKEY_CTX *keyCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
  EVP_PKEY_keygen_init(keyCtx);
  EVP_PKEY_CTX_set_rsa_keygen_bits(keyCtx, keySize);
  EVP_PKEY *key = NULL;
  EVP_PKEY_keygen(keyCtx, &key);
  EVP_PKEY_CTX_free(keyCtx);
  info("Key generation successful\n");

  // save private key
  BIO *privateKeyBio = BIO_new(BIO_s_mem());
  PEM_write_bio_PrivateKey(privateKeyBio, key, NULL, NULL, 0, 0, NULL);
  int privateKeyLength = BIO_pending(privateKeyBio);
  privateKey = PEM_read_bio_RSAPrivateKey(privateKeyBio, NULL, NULL, NULL);
  BIO_free_all(privateKeyBio);

  // extract public key
  BIO *publicKeyBio = BIO_new(BIO_s_mem());
  PEM_write_bio_PUBKEY(publicKeyBio, key);
  int publicKeyLength = BIO_pending(publicKeyBio);
  unsigned char *publicKeyArr = new unsigned char[publicKeyLength];
  memset(publicKeyArr, '\0', publicKeyLength);
  BIO_read(publicKeyBio, publicKeyArr, publicKeyLength);
  publicKey = string((char *)publicKeyArr);
  hashId = sha256(publicKey);

  info("Client hash id: " + hashId + "\n"); 

  BIO_free_all(publicKeyBio);
}

//encrypt message
string SslHandler::encryptMessage(string message, string publicKey)
{
  //read public key
  BIO *bio = BIO_new_mem_buf((void *)publicKey.c_str(), -1);
  RSA *publicRsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
  BIO_free_all(bio);

  //calculates limits
  int rsaLengthLimit = RSA_size(publicRsa);
  int blockLength = rsaLengthLimit - 11;

  //encrypts message in chunks with public key
  int iter = 0;
  string encryptedMessage = "";
  while (iter * blockLength < message.length())
  {
    string substring = message.substr(iter * blockLength, blockLength);
    unsigned char *block = (unsigned char *)malloc(rsaLengthLimit);
    memset(block, 0, rsaLengthLimit);
    RSA_public_encrypt(blockLength, (unsigned char *)substring.c_str(), block, publicRsa, RSA_PKCS1_PADDING);
    if (iter != 0)
    {
      encryptedMessage.push_back('#');
    }
    encryptedMessage += encodeBase64(block, rsaLengthLimit);
    iter++;
  }

  RSA_free(publicRsa);

  return encryptedMessage;
}

//decrypt message
string SslHandler::decryptMessage(string encryptedMessageEncoded)
{
  int rsaLengthLimit = RSA_size(privateKey);

  //segmentalize encoded string
  stringstream encryptedBlockStream(encryptedMessageEncoded);
  string segment;
  vector<string> encryptedBlocks;
  encryptedBlocks.clear();
  while (getline(encryptedBlockStream, segment, '#'))
  {
    encryptedBlocks.push_back(segment);
  }

  //decrypts message in chunks with private key
  string decryptedMessage = "";
  for (unsigned int i = 0; i < encryptedBlocks.size(); i++)
  {
    string encryptedBlock = decodeBase64(encryptedBlocks[i]);
    unsigned char *decryptedBlock = new unsigned char[rsaLengthLimit];
    memset(decryptedBlock, '\0', rsaLengthLimit);
    RSA_private_decrypt(rsaLengthLimit, (unsigned char *)encryptedBlock.c_str(), decryptedBlock, privateKey, RSA_PKCS1_PADDING);
    decryptedMessage += string((char *)decryptedBlock);
  }

  return decryptedMessage;
}

//sign message
string SslHandler::signMessage(string message)
{
  //calculate limits
  int rsaLengthLimit = RSA_size(privateKey);
  int blockLength = rsaLengthLimit - 11;

  //generate sha256 digest
  string messageHash = sha256(message);

  //encrypt digest with private key
  unsigned char *block = (unsigned char *)malloc(rsaLengthLimit);
  memset(block, 0, rsaLengthLimit);
  RSA_private_encrypt(blockLength, (unsigned char *)messageHash.c_str(), block, privateKey, RSA_PKCS1_PADDING);

  return encodeBase64(block, rsaLengthLimit);
}

//verify signature
bool SslHandler::verifyMessage(string message, string encodedSignature, string publicKey)
{
  //read publi ckey
  BIO *bio = BIO_new_mem_buf((void *)publicKey.c_str(), -1);
  RSA *publicRsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
  BIO_free_all(bio);

  //calculate limits
  int rsaLengthLimit = RSA_size(publicRsa);

  //generate sha256 digest
  string messageHash = sha256(message);

  //decrypt digest with public key
  string signature = decodeBase64(encodedSignature);
  unsigned char *decryptedBlock = new unsigned char[rsaLengthLimit];
  memset(decryptedBlock, '\0', rsaLengthLimit);
  RSA_public_decrypt(rsaLengthLimit, (unsigned char *)signature.c_str(), decryptedBlock, publicRsa, RSA_PKCS1_PADDING);
  string verification = string((char *)decryptedBlock);

  //return comparison result
  return verification == messageHash;
}