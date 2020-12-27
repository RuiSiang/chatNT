#include "listener_thread.h"
#include "socket_control.h"
#include "logger.h"
#include "ssl_handler.h"
#include "base64.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <stdint.h>
#include <signal.h>
#include <cstdlib>

#ifdef _WIN32
#include <WinSock2.h>
#endif

using namespace std;

//functions
int getBalance();
vector<string> getList();

//global variables
SocketControl *mainSocketControl;

int main(int argc, char *argv[])
{
#ifdef WIN32
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  //initialization start
  mainSocketControl = new SocketControl;

  char ip[100];
  int port;
  cout << "Please input relay server IP (v4): ";
  cin >> ip;
  cout << "Please input relay server port: ";
  cin >> port;
  if (mainSocketControl->bind(ip, port) == -1)
  {
    cout << "Connection failed, please check your ip and port input\n";
    exit(0);
  }

  SslHandler sslHandler;

  short int clientPort = 1024;
  while (CheckPortTCP(clientPort))
  {
    clientPort++;
  }
  ListenerThread *masterListenThread = new ListenerThread(clientPort, mainSocketControl);
  info("P2P service initialized at port " + to_string(clientPort) + "\n");
  thread t1(&ListenerThread::startListen, masterListenThread);
  string receiveString = "";
  string sendString = "REGISTER#" + sslHandler.getHashId() + "#" + to_string(clientPort) + "#" + sslHandler.getPublicKey();
  receiveString = mainSocketControl->sendCommand(sendString);
  info("Client registered at relay server\n");
  //initialization end

  //functionals start

  while (true)
  {
    string command;
    cout << "Input test command: ";
    cin >> command;
    if (command == "EXIT")
    {
      string sendString = "EXIT";
      string receiveString = mainSocketControl->sendCommand(sendString);
      exit(-1);
    }
    else
    {
      string sendString = command;
      string receiveString = mainSocketControl->sendCommand(sendString);
      cout << receiveString;
    }
  }
  //functionals end

  t1.join();
  cout << "Socket closed\n";
  delete mainSocketControl;
#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
