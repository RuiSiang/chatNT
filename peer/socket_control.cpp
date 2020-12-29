#include "socket_control.h"
#include "logger.h"

#include <cstring>
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

//checks if port is in use
bool CheckPortTCP(short int testPort)
{
  //attempts to create socket and closes socket if successful
  struct sockaddr_in client;
  int testDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  client.sin_family = AF_INET;
  client.sin_port = htons(testPort);
  client.sin_addr.s_addr = inet_addr("127.0.0.1");
  testDescriptor = (int)socket(AF_INET, SOCK_STREAM, 0);
  if (testDescriptor == -1)
  {
    return false;
  }
  int result = connect(testDescriptor, (struct sockaddr *)&client, sizeof(client));
  if (result == -1)
  {
    return false;
  }
  else
  {
#ifdef __linux__
    close(testDescriptor);
#elif _WIN32
    closesocket(testDescriptor);
#endif
    return true;
  }
}

//constructor
SocketControl::SocketControl()
{
  //create socket
  socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDescriptor == -1)
  {
    throw "P2P Socket Creation Failed\n";
    delete this;
  }
}

//bind socket ip and port
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

//sends command with socket
string SocketControl::sendCommand(string sendString)
{
  //sends data in chunks
  char sendData[CHUNK_SIZE];
  int iter = 0;
  while (iter * (CHUNK_SIZE - 1) < sendString.length())
  {
    string substring = sendString.substr(iter * (CHUNK_SIZE - 1), (CHUNK_SIZE - 1));
    memset(sendData, '\0', CHUNK_SIZE);
    strncpy(sendData, substring.c_str(), substring.length());
    send(socketDescriptor, sendData, CHUNK_SIZE, 0);
    iter++;
  }

  //receives data in chunks
  char receiveData[CHUNK_SIZE];
  string receiveString = "";
  while (true)
  {
    memset(receiveData, '\0', sizeof(receiveData));
    int recvErr = recv(socketDescriptor, receiveData, sizeof(receiveData), 0);
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

//destructor
SocketControl::~SocketControl()
{
#ifdef __linux__
  close(socketDescriptor);
#elif _WIN32
  closesocket(socketDescriptor);
#endif
}