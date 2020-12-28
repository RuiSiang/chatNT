// handler_thread.h
#ifndef HANDLER_THREAD_H
#define HANDLER_THREAD_H

#include <string>
#include <vector>
#include "listener_thread.h"
#include "logger.h"

class HandlerThread
{
public:
  HandlerThread(int descriptor, std::vector<User> *_user, std::string _ip)
  {
    threadSocketDescriptor = descriptor;
    user = _user;
    ip = _ip;
  }
  void handler();

private:
  int threadSocketDescriptor;
  int process(std::string);
  std::vector<User> *user;
  std::string ip;
  std::string hashId;
  void sendMessage(std::string);
  std::string receiveMessage(void);
};

#endif
