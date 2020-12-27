#include "handler_thread.h"
#include "logger.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <thread>
#include <unistd.h>

#define CHUNK_SIZE 1024

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#elif _WIN32
#include <WinSock2.h>
#endif

using namespace std;

void HandlerThread::handler()
{
  char sendData[CHUNK_SIZE], receiveData[CHUNK_SIZE];
  string sendString, receiveString;
  memset(sendData, '\0', sizeof(sendData));
  sendString = "Handler Assigned\n";
  sendMessage(sendString);
  while (true)
  {
    receiveString = receiveMessage();
    if (receiveString != "SIGFAULT")
    {
      if (process(receiveString))
      {
        break;
      }
    }
    else
    {
      break;
    }
  }
  memset(sendData, '\0', sizeof(sendData));
  sendString = "Payment received, retransmitting to server\n";
  strncpy(sendData, sendString.c_str(), sizeof(sendData));
  send(threadSocketDescriptor, sendData, sizeof(sendData), 0);

  delete this;
}

HandlerThread::~HandlerThread()
{
  closesocket(threadSocketDescriptor);
}

int HandlerThread::process(string receiveString)
{
  info(receiveString + "asdf");
  return 0;
}

string HandlerThread::receiveMessage(void)
{
  char receiveData[CHUNK_SIZE];
  string receiveString = "";
  while (true)
  {
    memset(receiveData, '\0', sizeof(receiveData));
    int recvErr = recv(threadSocketDescriptor, receiveData, sizeof(receiveData), 0);
    if (recvErr < 0)
    {
      return "SIGFAULT";
    }
    receiveString += string(receiveData);
    if (string(receiveData).length() < CHUNK_SIZE)
    {
      break;
    }
  }
  return receiveString;
}

void HandlerThread::sendMessage(string sendString)
{
  char sendData[CHUNK_SIZE];
  int iter = 0;
  while (iter * CHUNK_SIZE < sendString.length())
  {
    string substring = sendString.substr(iter * CHUNK_SIZE, CHUNK_SIZE);
    memset(sendData, '\0', sizeof(sendData));
    strncpy(sendData, substring.c_str(), substring.length());
    send(threadSocketDescriptor, sendData, sizeof(sendData), 0);
    iter++;
  }
}
