#include "socket_control.h"
#include "logger.h"

#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

#define CHUNK_SIZE 100

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#elif _WIN32
#include <WinSock2.h>
#endif

using namespace std;

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
  memset(sendData, '\0', sizeof(sendData));
  strncpy(sendData, sendString.c_str(), sizeof(sendData));
  send(socketDescriptor, sendData, sizeof(sendData), 0);
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

void SocketControl::terminate()
{
  close(socketDescriptor);
}