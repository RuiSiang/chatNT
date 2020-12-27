#include "socket_control.h"
#include "logger.h"

#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

#define CHUNK_SIZE 1024

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#elif _WIN32
#include <WinSock2.h>
#endif

using namespace std;

bool CheckPortTCP(short int testPort)
{
  struct sockaddr_in client;
  int testDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  client.sin_family = AF_INET;
  client.sin_port = htons(testPort);
  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  testDescriptor = (int)socket(AF_INET, SOCK_STREAM, 0);
  if (testDescriptor == INVALID_SOCKET)
  {
    return false;
  }
  int result = connect(testDescriptor, (struct sockaddr *)&client, sizeof(client));
  if (result == SOCKET_ERROR)
  {
    return false;
  }
  else
  {
    closesocket(testDescriptor);
    return true;
  }
}

SocketControl::SocketControl()
{
  socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDescriptor == -1)
  {
    throw "P2P Socket Creation Failed\n";
    delete this;
  }
}

int SocketControl::bind(char ip[100], int port)
{
  struct sockaddr_in connectionInfo;
  memset(&connectionInfo, 0, sizeof(connectionInfo));
  connectionInfo.sin_family = PF_INET;
  connectionInfo.sin_addr.s_addr = inet_addr(ip);
  connectionInfo.sin_port = htons(port);
  int err = connect(socketDescriptor, (struct sockaddr *)&connectionInfo, sizeof(connectionInfo));
  if (err == -1)
  {
    return -1;
  }
  else
  {
    char welcomeData[CHUNK_SIZE];
    memset(welcomeData, '\0', sizeof(welcomeData));
    recv(socketDescriptor, welcomeData, sizeof(welcomeData), 0);
    string welcomeString = string(welcomeData);
    info(welcomeString);
    return 0;
  }
}

string SocketControl::sendCommand(string sendString)
{
  char sendData[CHUNK_SIZE], receiveData[CHUNK_SIZE];
  int iter = 0;
  while (iter * CHUNK_SIZE < sendString.length())
  {
    string substring = sendString.substr(iter * CHUNK_SIZE, CHUNK_SIZE);
    memset(sendData, '\0', sizeof(sendData));
    strncpy(sendData, sendString.c_str(), substring.length());
    send(socketDescriptor, sendData, sizeof(sendData), 0);
    iter++;
  }
  string receiveString = "";
  while (true)
  {
    memset(receiveData, '\0', sizeof(receiveData));
    recv(socketDescriptor, receiveData, sizeof(receiveData), 0);
    receiveString += string(receiveData);
    if (string(receiveData).length() < CHUNK_SIZE)
    {
      break;
    }
  }
  return receiveString;
}

SocketControl::~SocketControl()
{
  closesocket(socketDescriptor);
}