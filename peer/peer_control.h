// peer_control.h
#ifndef PEER_CONTROL_H
#define PEER_CONTROL_H
#include "socket_control.h"
#include "listener_thread.h"
#include "ssl_handler.h"

#include <vector>
#include <string>
#include <thread>

class PeerControl
{
public:
  PeerControl(char[100], unsigned int);
  ~PeerControl();
  void terminate();
  void updateList(void);
  User getUser(std::string);
  bool userExists(std::string);
  bool formPacketandSend(std::string, std::string);
  std::vector<User> getList() { return userList; }
  std::vector<Message> getMessages() { return messages; }

private:
  SocketControl *mainSocketControl;
  ListenerThread *masterListenThread;
  SslHandler sslHandler;
  std::vector<User> userList;
  std::thread _thread;
  std::vector<Message> messages;
  std::string sendMessage(std::string, char[100], short);
};

#endif