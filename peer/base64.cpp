#include "base64.h"

#include <algorithm>

using namespace std;

//define charset
static const char *charSet = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

//transform charset back to short
static unsigned int transform(const unsigned char character)
{
  if (character >= 'A' && character <= 'Z')
    return character - 'A';
  else if (character >= 'a' && character <= 'z')
    return character - 'a' + ('Z' - 'A') + 1;
  else if (character >= '0' && character <= '9')
    return character - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
  else if (character == '+')
    return 62;
  else if (character == '/')
    return 63;
  return 0;
}

//encode unsigned char array to base64 string
string encodeBase64(unsigned char *bytes, int bytesLength)
{
  string encodedString;
  unsigned int iter = 0;
  while (iter < bytesLength)
  {
    encodedString.push_back(charSet[(bytes[iter] & 0xfc) >> 2]);
    encodedString.push_back(charSet[((bytes[iter] & 0x03) << 4) + ((bytes[iter + 1] & 0xf0) >> 4)]);
    encodedString.push_back(charSet[((bytes[iter + 1] & 0x0f) << 2) + ((bytes[iter + 2] & 0xc0) >> 6)]);
    encodedString.push_back(charSet[bytes[iter + 2] & 0x3f]);
    iter += 3;
  }
  encodedString.push_back('=');
  encodedString.push_back('=');
  return encodedString;
}

//decode base64 string to string
string decodeBase64(const string encodedString)
{
  if (encodedString.empty())
  {
    return string();
  }
  size_t encodedLength = encodedString.length();
  size_t iter = 0;
  size_t decodedLength = encodedLength / 4 * 3;
  string decodedString;
  decodedString.reserve(decodedLength);
  while (iter < encodedLength)
  {
    unsigned int nextChar = transform(encodedString[iter + 1]);
    decodedString.push_back(static_cast<string::value_type>(((transform(encodedString[iter + 0])) << 2) + ((nextChar & 0x30) >> 4)));
    if (encodedString[iter + 2] != '=')
    {
      unsigned int nextNextChar = transform(encodedString[iter + 2]);
      decodedString.push_back(static_cast<string::value_type>(((nextChar & 0x0f) << 4) + ((nextNextChar & 0x3c) >> 2)));
      if (encodedString[iter + 3] != '=')
      {
        decodedString.push_back(static_cast<string::value_type>(((nextNextChar & 0x03) << 6) + transform(encodedString[iter + 3])));
      }
    }
    iter += 4;
  }
  return decodedString;
}