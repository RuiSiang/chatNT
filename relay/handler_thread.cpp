#include "handler_thread.h"

#include <iostream>
#include <sstream>
#include <cstring>
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

void HandlerThread::handler()
{
  char sendData[CHUNK_SIZE], receiveData[CHUNK_SIZE];
  string sendString, receiveString;
  memset(sendData, '\0', sizeof(sendData));
  sendString = "Handler Assigned\n";
  strncpy(sendData, sendString.c_str(), sizeof(sendData));
  send(threadSocketDescriptor, sendData, sizeof(sendData), 0);
  while (true)
  {
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
    if (process(receiveString))
    {
      break;
    }
  }
  info("Thread " + to_string(threadSocketDescriptor) + " terminated\n");
  close(threadSocketDescriptor);

  delete this;
}

int HandlerThread::process(string receiveString)
{
  if (receiveString == "Exit")
  {
    for (int i = 0; i < dataset->size(); i++)
    {
      if (dataset->at(i).username == username)
      {
        dataset->at(i).active = 0;
      }
    }
    char sendData[CHUNK_SIZE];
    string sendString = "Bye";
    memset(sendData, '\0', sizeof(sendData));
    strncpy(sendData, sendString.c_str(), sizeof(sendData));
    send(threadSocketDescriptor, sendData, sizeof(sendData), 0);
    return 1;
  }
  else
  {
    cout << receiveString;
  }
  return 0;
}
