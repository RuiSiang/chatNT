#include "peer_control.h"
#include "logger.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <vector>

using namespace std;

//test driver program
int main(int argc, char *argv[])
{
  //initialization
  char ip[100];
  unsigned int port;
  cout << "Please input relay server IP (v4): ";
  cin >> ip;
  cout << "Please input relay server port: ";
  cin >> port;

  PeerControl *peerControl = new PeerControl(ip, port);

  //functionals
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
      peerControl->updateList();
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
    else if (command == "send")
    {
      string receiver, message;
      cout << "Please input receiver hash id: ";
      cin >> receiver;
      peerControl->updateList();
      while (!peerControl->userExists(receiver))
      {
        peerControl->updateList();
        cout << "User hash id non-existent. Please try again: ";
        cin >> receiver;
      }
      cout << "Please input message to send:\n";
      cin.ignore();
      message = "\n";
      getline(cin, message);
      bool sendResult = peerControl->formPacketandSend(receiver, message);
      if (!sendResult)
      {
        info("Message sent successfully\n");
      }
      else
      {
        error("Message send failed\n");
      }
    }
    else if (command == "messages")
    {
      peerControl->updateList();
      vector<Message> messages = peerControl->getMessages();
      cout << "================================================================\n";
      for (unsigned int i = 0; i < messages.size(); i++)
      {
        if (i != 0)
        {
          cout << "----------------------------------------------------------------\n";
        }
        cout << messages[i].hashId << "\n";
        cout << messages[i].message << "\n";
      }
      cout << "================================================================\n";
    }
  }

  //closing
  delete peerControl;

  return 0;
}
