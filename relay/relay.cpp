#include "listener_thread.h"

#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#endif

#define CHUNK_SIZE 1024

using namespace std;

//driver program
int main(int argc, char *argv[])
{
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  unsigned int port;
  cout<<"chatNT - Anonymous Chatroom\n"
  cout << "Please input server port: ";
  cin >> port;

  ListenerThread masterThread(port);
  masterThread.startListen(10);

  return 0;
}