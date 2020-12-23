#include "handler_thread.h"
#include "logger.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <thread>
#include <unistd.h>

#define CHUNK_SIZE 100

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
  strncpy(sendData, sendString.c_str(), sizeof(sendData));
  send(threadSocketDescriptor, sendData, sizeof(sendData), 0);
  receiveString = "";
  while (true)
  {
    memset(receiveData, '\0', sizeof(receiveData));
    recv(threadSocketDescriptor, receiveData, sizeof(receiveData), 0);
    receiveString += string(receiveData);
    if (string(receiveData).length() < CHUNK_SIZE)
    {
      break;
    }
  }
  process(receiveString);
  memset(sendData, '\0', sizeof(sendData));
  sendString = "Payment received, retransmitting to server\n";
  strncpy(sendData, sendString.c_str(), sizeof(sendData));
  send(threadSocketDescriptor, sendData, sizeof(sendData), 0);

  info("Thread " + to_string(threadSocketDescriptor) + " terminated\n");
  close(threadSocketDescriptor);

  delete this;
}

int HandlerThread::process(string receiveString)
{
  info(receiveString);
  return 0;
}
