#include "peer_control.h"
#include "logger.h"

#include <cstring>
#include <sstream>
#include <cstdlib>

using namespace std;

#ifdef _WIN32
#include <WinSock2.h>
#endif

//constructor
PeerControl::PeerControl(char ip[100], unsigned int port)
{
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  //initialize variables
  userList.clear();
  messages.clear();
  hashId = sslHandler.getHashId();

  //create socket
  mainSocketControl = new SocketControl;
  if (mainSocketControl->bind(ip, port) == -1)
  {
    error("Connection failed, please check your ip and port input\n");
    exit(0);
  }

  //init master listen thread
  short int clientPort = 1024;
  while (CheckPortTCP(clientPort))
  {
    clientPort++;
  }
  masterListenThread = new ListenerThread(clientPort, mainSocketControl, &sslHandler, &userList, &messages);
  _thread = thread(&ListenerThread::startListen, masterListenThread);
  info("P2P service initialized at port " + to_string(clientPort) + "\n");

  //register info on relay server
  string sendString = "REGISTER#" + sslHandler.getHashId() + "#" + to_string(clientPort) + "#" + sslHandler.getPublicKey();
  string receiveString = mainSocketControl->sendCommand(sendString);
  info("Client registered at relay server\n");
}

//destructor
PeerControl::~PeerControl()
{
  _thread.join();
  delete mainSocketControl;
#ifdef _WIN32
  WSACleanup();
#endif
}

//terminates connection to relay server
void PeerControl::terminate()
{
  string sendString = "EXIT";
  string receiveString = mainSocketControl->sendCommand(sendString);
  info("Exiting\n");
}

//syncs user list from relay server
void PeerControl::updateList()
{
  //sends LIST call to relay server
  string sendString = "LIST";
  string receiveString = mainSocketControl->sendCommand(sendString);

  //segmemtalizes response
  stringstream receiveStream(receiveString);
  string segment;
  vector<string> segments;
  segments.clear();
  while (getline(receiveStream, segment, '#'))
  {
    segments.push_back(segment);
  }

  //updates user list
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

//gets user object with hash id
User PeerControl::getUser(string hashId)
{
  for (unsigned int i = 0; i < userList.size(); i++)
  {
    if (userList[i].hashId == hashId)
    {
      return userList[i];
    }
  }
}

//checks if user exists
bool PeerControl::userExists(string hashId)
{
  for (unsigned int i = 0; i < userList.size(); i++)
  {
    if (userList[i].hashId == hashId)
    {
      return true;
    }
  }
  return false;
}

//forms packet and sends it
bool PeerControl::formPacketandSend(std::string receiverHashId, std::string message)
{
  //ensure newest user list is obtained
  updateList();

  //gets user objects needed
  User receiver = getUser(receiverHashId);
  User peer1 = userList[rand() % userList.size()];
  User peer2 = userList[rand() % userList.size()];
  User peer3 = userList[rand() % userList.size()];

  //validates public key with associated hash id to prevent corrupted server or MITM attacks
  if (!(sha256(peer1.publicKey) == peer1.hashId && sha256(peer2.publicKey) == peer2.hashId &&
        sha256(peer3.publicKey) == peer3.hashId && sha256(receiver.publicKey) == receiver.hashId))
  {
    return 1;
  }

  //encrypts sections with public keys corresponding to the expected reader
  string peer1Section = sslHandler.encryptMessage(peer2.hashId, peer1.publicKey);
  string peer2Section = sslHandler.encryptMessage(peer3.hashId, peer2.publicKey);
  string secretPacket = sslHandler.encryptMessage(
      sslHandler.getHashId() + "%" + message + "%" + sslHandler.signMessage(message), receiver.publicKey);
  string peer3Section = sslHandler.encryptMessage(receiverHashId + "%" + secretPacket, peer3.publicKey);

  //forms packet and sends it to peer1
  string packet = peer1Section + "%" + peer2Section + "%" + peer3Section;
  bool sendResult = sendMessage("P1%" + packet, (char *)peer1.ip.c_str(), peer1.port) == "SIGFAULT";
  if (!sendResult)
  {
    Message tmp;
    tmp.receiver = receiverHashId;
    tmp.sender = sslHandler.getHashId();
    tmp.message = message;
    messages.push_back(tmp);
    return 0;
  }
  else
  {
    return 1;
  }
}

//send message to specified location
string PeerControl::sendMessage(string sendString, char ip[100], short port)
{
  SocketControl *tmpSocketControl = new SocketControl;
  if (tmpSocketControl->bind(ip, port) == -1)
  {
    return "SIGFAULT";
  }
  string receiveString = tmpSocketControl->sendCommand(sendString);
  return receiveString;
}