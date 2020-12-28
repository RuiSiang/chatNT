// peer_control.h
#ifndef PEER_CONTROL_H
#define PEER_CONTROL_H
#include "socket_control.h"
#include "listener_thread.h"
#include "ssl_handler.h"

#include <vector>
#include <string>
#include <thread>

//main control interface for peer
class PeerControl
{
public:
  PeerControl(char[100], unsigned int); //constructor
  ~PeerControl(); //destructor
  void terminate(); //terminates connection to relay server
  void updateList(); //syncs user list with relay server
  User getUser(std::string); //gets user object with hash id
  bool userExists(std::string); //checks if user exists
  bool formPacketandSend(std::string, std::string); //forms packet and sends it
  std::vector<User> getList() { return userList; } //returns user list
  std::vector<Message> getMessages() { return messages; } //returns all messages

private:
  SocketControl *mainSocketControl; //socket file descriptor for connection to relay server
  ListenerThread *masterListenThread; //master listen thread object
  SslHandler sslHandler; //ssl object for crypto operations
  std::vector<User> userList; //saves user list
  std::thread _thread; //thread handler object
  std::vector<Message> messages; //saves all messages
  std::string sendMessage(std::string, char[100], short); //send message to specified location
};

#endif