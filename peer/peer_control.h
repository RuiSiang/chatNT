// peer_control.h
#ifndef PEER_CONTROL_H
#define PEER_CONTROL_H
#include "socket_control.h"
#include "listener_thread.h"
#include "ssl_handler.h"

#include <vector>
#include <string>
#include <thread>

struct User
{
  std::string hashId;
  std::string ip;
  int port;
  std::string publicKey;
};

class PeerControl
{
public:
  PeerControl(char[100], unsigned int);
  ~PeerControl();
  void terminate();
  std::vector<User> getList();

private:
  SocketControl *mainSocketControl;
  ListenerThread *masterListenThread;
  SslHandler sslHandler;
  std::vector<User> userList;
  void updateList(void);
  std::thread _thread;
};

#endif