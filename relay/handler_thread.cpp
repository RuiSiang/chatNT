#include "handler_thread.h"

#include <sstream>
#include <cstring>
#include <thread>
#include <unistd.h>
#include <iostream>
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

void HandlerThread::handler()
{
  string sendString, receiveString;
  sendString = "Connection initialized\n";
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
  for (unsigned int i = 0; i < user->size(); i++)
  {
    if (user->at(i).hashId == hashId)
    {
      user->erase(user->begin() + i);
      break;
    }
  }
  info("Thread " + to_string(threadSocketDescriptor) + "with hash id: " + hashId + " terminated\n");
  close(threadSocketDescriptor);

  delete this;
}

int HandlerThread::process(string receiveString)
{
  stringstream receiveStream(receiveString);
  string segment;
  vector<string> segments;
  segments.clear();
  while (getline(receiveStream, segment, '#'))
  {
    segments.push_back(segment);
  }
  if (segments[0] == "EXIT")
  {
    sendMessage("Bye");
    for (unsigned int i = 0; i < user->size(); i++)
    {
      if (user->at(i).hashId == hashId)
      {
        user->erase(user->begin() + i);
        break;
      }
    }
    return 1;
  }
  else if (segments[0] == "REGISTER")
  {
    User tmp;
    tmp.hashId = segments[1];
    hashId = segments[1];
    tmp.ip = ip;
    tmp.port = stoi(segments[2]);
    tmp.publicKey = segments[3];
    user->push_back(tmp);
    sendMessage("Register Successful");
  }
  else if (segments[0] == "LIST")
  {
    string dataString = "";
    for (unsigned int i = 0; i < user->size(); i++)
    {
      if (i != 0)
      {
        dataString += "#";
      }
      dataString += user->at(i).hashId + "#" + user->at(i).ip + "#" + to_string(user->at(i).port) + "#" + user->at(i).publicKey;
    }
    sendMessage(dataString);
  }
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
    if (string(receiveData).length() < CHUNK_SIZE - 1)
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
  while (iter * (CHUNK_SIZE - 1) < sendString.length())
  {
    string substring = sendString.substr(iter * (CHUNK_SIZE - 1), (CHUNK_SIZE - 1));
    memset(sendData, '\0', CHUNK_SIZE);
    strncpy(sendData, substring.c_str(), substring.length());
    send(threadSocketDescriptor, sendData, CHUNK_SIZE, 0);
    iter++;
  }
}