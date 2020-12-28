// base64.h
#ifndef BASE64_H
#define BASE64_H

#include <string>

std::string encodeBase64(unsigned char *, int); //encode unsigned char array to base64 string
std::string decodeBase64(std::string const); //decode base64 string to string

#endif