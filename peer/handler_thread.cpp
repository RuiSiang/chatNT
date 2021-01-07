#include "handler_thread.h"
#include "logger.h"

#include <cstring>
#include <sstream>
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

//main handler for thread
void HandlerThread::handler()
{
  string sendString, receiveString;
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
  sendString = "Transmission received\n";
  sendMessage(sendString);
  delete this;
}

//destructor
HandlerThread::~HandlerThread()
{
#ifdef __linux__
  close(threadSocketDescriptor);
#elif _WIN32
  closesocket(threadSocketDescriptor);
#endif
}

//processor for received data
int HandlerThread::process(string receiveString)
{
  if (receiveString != "")
  {
    //incoming data segmentalization
    stringstream receiveStream(receiveString);
    string segment;
    vector<string> segments;
    segments.clear();
    while (getline(receiveStream, segment, '%'))
    {
      segments.push_back(segment);
    }

    info("Retransmitting packet with tag " + segments[0] + "\n");

    //ensure newest user list is attained
    updateList();

    //evaluate command and undergo corresponding actions
    if (segments[0] == "P1")
    {
      //evaluates information validity
      string peer2HashId = sslHandler->decryptMessage(segments[1]);
      if (!userExists(peer2HashId))
      {
        sendMessage("SIGFAULT");
        return 0;
      }
      User peer2 = getUser(peer2HashId);
      if (!(sha256(peer2.publicKey) == peer2.hashId))
      {
        sendMessage("SIGFAULT");
        return 0;
      }

      //send peer2 and peer3 sections to p2
      bool sendResult = sendMessage("P2%" + segments[2] + "%" + segments[3], (char *)peer2.ip.c_str(), peer2.port) == "SIGFAULT";
      if (!sendResult)
      {
        sendMessage("SUCCESS");
      }
      else
      {
        sendMessage("SIGFAULT");
      }
    }
    else if (segments[0] == "P2")
    {
      //validates information validity
      string peer3HashId = sslHandler->decryptMessage(segments[1]);
      if (!userExists(peer3HashId))
      {
        sendMessage("SIGFAULT");
        return 0;
      }
      User peer3 = getUser(peer3HashId);
      if (!(sha256(peer3.publicKey) == peer3.hashId))
      {
        sendMessage("SIGFAULT");
        return 0;
      }

      //send peer3 section to p3
      bool sendResult = sendMessage("P3%" + segments[2], (char *)peer3.ip.c_str(), peer3.port) == "SIGFAULT";
      if (!sendResult)
      {
        sendMessage("SUCCESS");
      }
      else
      {
        sendMessage("SIGFAULT");
      }
    }
    else if (segments[0] == "P3")
    {
      //decrypt and segmentalize peer3 section
      string peer3Section = sslHandler->decryptMessage(segments[1]);
      stringstream peer3SectionStream(peer3Section);
      string block;
      vector<string> blocks;
      blocks.clear();
      while (getline(peer3SectionStream, block, '%'))
      {
        blocks.push_back(block);
      }

      //evaluates information validity
      string receiverHashId = blocks[0];
      if (!userExists(receiverHashId))
      {
        sendMessage("SIGFAULT");
        return 0;
      }
      User receiver = getUser(receiverHashId);

      //send secret packet to receiver
      bool sendResult = sendMessage("R%" + blocks[1], (char *)receiver.ip.c_str(), receiver.port) == "SIGFAULT";
      if (!sendResult)
      {
        sendMessage("SUCCESS");
      }
      else
      {
        sendMessage("SIGFAULT");
      }
    }
    else if (segments[0] == "R")
    {
      //decrypt and segmentalize secret packet
      string secretPacket = sslHandler->decryptMessage(segments[1]);
      stringstream secretPacketStream(secretPacket);
      string block;
      vector<string> blocks;
      blocks.clear();
      while (getline(secretPacketStream, block, '%'))
      {
        blocks.push_back(block);
      }

      //evaluated information validity
      string senderHashId = blocks[0];
      if (!userExists(senderHashId))
      {
        sendMessage("SIGFAULT");
        return 0;
      }
      User sender = getUser(senderHashId);

      //validates signature and saves message object if signature check passes
      bool validSignature = sslHandler->verifyMessage(blocks[1], blocks[2], sender.publicKey);
      if (validSignature)
      {
        if (sender.hashId != sslHandler->getHashId())
        {
          Message tmp;
          tmp.sender = sender.hashId;
          tmp.receiver = sslHandler->getHashId();
          tmp.message = blocks[1];
          messages->push_back(tmp);
        }
        sendMessage("SUCCESS");
      }
      else
      {
        sendMessage("SIGFAULT");
      }
    }
  }
  return 0;
}

//wait for data and return received string
string HandlerThread::receiveMessage()
{
  //receives data in chunks and collects into a string to return
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

//send data to relay server
void HandlerThread::sendMessage(string sendString)
{
  //sends data in chunks
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

//send data to specified location
string HandlerThread::sendMessage(string sendString, char ip[100], short port)
{
  SocketControl *tmpSocketControl = new SocketControl;
  if (tmpSocketControl->bind(ip, port) == -1)
  {
    return "SIGFAULT";
  }
  string receiveString = tmpSocketControl->sendCommand(sendString);
  return receiveString;
}

//get user object with hash id
User HandlerThread::getUser(string hashId)
{
  for (unsigned int i = 0; i < userList->size(); i++)
  {
    if (userList->at(i).hashId == hashId)
    {
      return userList->at(i);
    }
  }
}

//checks if user with specified hash id exists
bool HandlerThread::userExists(string hashId)
{
  for (unsigned int i = 0; i < userList->size(); i++)
  {
    if (userList->at(i).hashId == hashId)
    {
      return true;
    }
  }
  return false;
}

//sync user list from relay server
void HandlerThread::updateList()
{
  //sends LIST call to relay server
  string sendString = "LIST";
  string receiveString = mainSocketControl->sendCommand(sendString);

  //segmentalizes response
  stringstream receiveStream(receiveString);
  string segment;
  vector<string> segments;
  segments.clear();
  while (getline(receiveStream, segment, '#'))
  {
    segments.push_back(segment);
  }

  //saves to userList object
  userList->clear();
  for (unsigned int i = 0; i < segments.size(); i += 4)
  {
    User tmp;
    tmp.hashId = segments[i];
    tmp.ip = segments[i + 1];
    tmp.port = stoi(segments[i + 2]);
    tmp.publicKey = segments[i + 3];
    userList->push_back(tmp);
  }
}