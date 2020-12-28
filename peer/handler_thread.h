// handler_thread.h
#ifndef HANDLER_THREAD_H
#define HANDLER_THREAD_H

#include <string>
#include <vector>
#include "socket_control.h"
#include "ssl_handler.h"

struct User
{
  std::string hashId;
  std::string ip;
  int port;
  std::string publicKey;
};

struct Message
{
  std::string hashId;
  std::string message;
};

class HandlerThread
{
public:
  HandlerThread(int descriptor, SocketControl *_mainsocketControl, SslHandler* _sslHandler, std::vector<User> *_userList, std::vector<Message> *_messages)
  {
    threadSocketDescriptor = descriptor;
    mainSocketControl = _mainsocketControl;
    sslHandler = _sslHandler;
    userList = _userList;
    messages = _messages;
  }
  ~HandlerThread();
  void handler();

private:
  int threadSocketDescriptor;
  SocketControl *mainSocketControl;
  SslHandler* sslHandler;
  std::vector<User> *userList;
  std::vector<Message> *messages;
  int process(std::string);
  void sendMessage(std::string);
  std::string receiveMessage(void);
  std::string sendMessage(std::string, char[100], short);
  User getUser(std::string);
  bool userExists(std::string);
  void updateList(void);
};

#endif