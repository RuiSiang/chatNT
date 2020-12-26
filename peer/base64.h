// base64.h
#ifndef BASE64_H
#define BASE64_H

#include <string>

std::string encodeBase64(unsigned char *, int);
std::string decodeBase64(std::string const);

#endif