#include "listener_thread.h"
#include "socket_control.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <stdint.h>

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
  //connection initialization start
  mainSocketControl = new SocketControl;
  while (true)
  {
    char ip[100];
    int port;
    cout << "Please input server IP (v4): ";
    cin >> ip;
    cout << "Please input server port: ";
    cin >> port;
    if (mainSocketControl->bind(ip, port) == -1)
    {
      cout << "Connection failed, please check your ip and port input\n";
    }
    else
    {
      break;
    }
  }
  //connection initialization end

  //functionals start
  int clientPort;
  while (true)
  {
    cout << "Client port to open: ";
    cin >> clientPort;
    if (clientPort >= 1024 && clientPort <= 65536)
    {
      break;
    }
  }
  ListenerThread *masterListenThread = new ListenerThread(clientPort, mainSocketControl);
  thread t1(&ListenerThread::startListen, masterListenThread);

  while (true)
  {
    string command;
    cout << "Input test command: ";
    cin >> command;
    if (command == "logout")
    {
      string sendString = "Exit";
      string receiveString = mainSocketControl->sendCommand(sendString);
      break;
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
  mainSocketControl->terminate();
#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
