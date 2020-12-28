// handler_thread.h
#ifndef HANDLER_THREAD_H
#define HANDLER_THREAD_H

#include <string>
#include <vector>
#include "listener_thread.h"
#include "logger.h"

//parallel socket thread to process incoming connections
class HandlerThread
{
public:
  //constructor
  HandlerThread(int descriptor, std::vector<User> *_user, std::string _ip)
  {
    threadSocketDescriptor = descriptor;
    user = _user;
    ip = _ip;
  }
  void handler(); //main thread handler

private:
  int threadSocketDescriptor; //socket file descriptor for thread
  int process(std::string); //processor for received data
  std::vector<User> *user; //saves user list
  std::string ip; //saves incoming connection IP
  std::string hashId; //saves hash id of incoming connection
  void sendMessage(std::string); //send message with socket
  std::string receiveMessage(); //wait to receive data
};

#endif
