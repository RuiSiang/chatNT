#include "peer_control.h"
#include "logger.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdlib>

using namespace std;

#ifdef _WIN32
#include <WinSock2.h>
#endif

PeerControl::PeerControl(char ip[100], unsigned int port)
{
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  //create socket start
  mainSocketControl = new SocketControl;
  if (mainSocketControl->bind(ip, port) == -1)
  {
    error("Connection failed, please check your ip and port input\n");
    exit(0);
  }
  //create socket end

  //init master listen thread start
  short int clientPort = 1024;
  while (CheckPortTCP(clientPort))
  {
    clientPort++;
  }
  masterListenThread = new ListenerThread(clientPort, mainSocketControl);
  _thread = thread(&ListenerThread::startListen, masterListenThread);
  info("P2P service initialized at port " + to_string(clientPort) + "\n");
  //init master listen thead end
  //register info start
  string sendString = "REGISTER#" + sslHandler.getHashId() + "#" + to_string(clientPort) + "#" + sslHandler.getPublicKey();
  string receiveString = mainSocketControl->sendCommand(sendString);
  info("Client registered at relay server\n");
  //register info end
}

PeerControl::~PeerControl()
{
  _thread.join();
  delete mainSocketControl;
#ifdef _WIN32
  WSACleanup();
#endif
}

void PeerControl::terminate()
{
  string sendString = "EXIT";
  string receiveString = mainSocketControl->sendCommand(sendString);
  info("Exiting\n");
}

void PeerControl::updateList(void)
{
  string sendString = "LIST";
  string receiveString = mainSocketControl->sendCommand(sendString);
  stringstream receiveStream(receiveString);
  string segment;
  vector<string> segments;
  segments.clear();
  while (getline(receiveStream, segment, '#'))
  {
    segments.push_back(segment);
  }
  userList.clear();
  for (unsigned int i = 0; i < segments.size(); i += 4)
  {
    User tmp;
    tmp.hashId = segments[i];
    tmp.ip = segments[i + 1];
    tmp.port = stoi(segments[i + 2]);
    tmp.publicKey = segments[i + 3];
    userList.push_back(tmp);
  }
}

vector<User> PeerControl::getList()
{
  updateList();
  return userList;
}