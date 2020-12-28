#include "peer_control.h"

#include <iostream>
#include <cstring>
#include <iomanip>
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#endif

using namespace std;

int main(int argc, char *argv[])
{
  //initialization start
  char ip[100];
  unsigned int port;
  cout << "Please input relay server IP (v4): ";
  cin >> ip;
  cout << "Please input relay server port: ";
  cin >> port;

  PeerControl *peerControl = new PeerControl(ip, port);
  //initialization end

  //functionals start

  while (true)
  {
    string command;
    cout << "Input test command: ";
    cin >> command;
    if (command == "exit")
    {
      peerControl->terminate();
      exit(0);
    }
    else if (command == "list")
    {
      vector<User> userList = peerControl->getList();
      cout << "================================================================\n";
      for (unsigned int i = 0; i < userList.size(); i++)
      {
        if (i != 0)
        {
          cout << "----------------------------------------------------------------\n";
        }
        cout << userList[i].hashId << "\n";
        cout << setw(32) << "IP: " + userList[i].ip << setw(32) << "Port: " + to_string(userList[i].port) << "\n";
        cout << userList[i].publicKey << "\n";
      }
      cout << "================================================================\n";
    }
  }
  //functionals end

  //closing start
  delete peerControl;
  //closeing end

  return 0;
}
