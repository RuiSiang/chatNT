#include "handler_thread.h"

#include <sstream>
#include <cstring>
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
  info("Thread " + to_string(threadSocketDescriptor) + " terminated\n");
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
    for (int i = 0; i < dataset->size(); i++)
    {
      if (dataset->at(i).hashId == hashId)
      {
        dataset->erase(dataset->begin() + i);
        break;
      }
    }
    return 1;
  }
  else if (segments[0] == "REGISTER")
  {
    Dataset tmp;
    tmp.hashId = segments[1];
    hashId = segments[1];
    tmp.ip = ip;
    tmp.port = stoi(segments[2]);
    tmp.publicKey = segments[3];
    dataset->push_back(tmp);
    sendMessage("Register Successful");
  }
  else if (segments[0] == "LIST")
  {
    string dataString = "";
    for (int i = 0; i < dataset->size(); i++)
    {
      if (i != 0)
      {
        dataString += "#";
      }
      dataString += dataset->at(i).hashId + "#" + dataset->at(i).ip + "#" + to_string(dataset->at(i).port) + "#" + dataset->at(i).publicKey;
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