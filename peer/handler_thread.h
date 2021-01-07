// handler_thread.h
#ifndef HANDLER_THREAD_H
#define HANDLER_THREAD_H

#include <string>
#include <vector>
#include "socket_control.h"
#include "ssl_handler.h"

//user object declaration
struct User
{
  std::string hashId;
  std::string ip;
  int port;
  std::string publicKey;
};

//message object declaration
struct Message
{
  std::string sender;
  std::string receiver;
  std::string message;
};

//parallel socket thread to process incoming connections
class HandlerThread
{
public:
  //constructor
  HandlerThread(int descriptor, SocketControl *_mainsocketControl, SslHandler* _sslHandler, std::vector<User> *_userList, std::vector<Message> *_messages)
  {
    threadSocketDescriptor = descriptor;
    mainSocketControl = _mainsocketControl;
    sslHandler = _sslHandler;
    userList = _userList;
    messages = _messages;
  }
  ~HandlerThread(); //destructor
  void handler(); //main handler

private:
  int threadSocketDescriptor; //socket file descriptor for thread
  SocketControl *mainSocketControl; //socket file descriptor for relay server conenction
  SslHandler* sslHandler; //ssl handler object for crypto operations
  std::vector<User> *userList; //saves all user information
  std::vector<Message> *messages; //saves all messages 
  int process(std::string); //processor for incoming data
  void sendMessage(std::string); //send message to relay server
  std::string receiveMessage(); //wait for receiving data and return received data
  std::string sendMessage(std::string, char[100], short); //send message to specified location
  User getUser(std::string); //get user by hash id
  bool userExists(std::string); //check if user exists
  void updateList(); //sync user list from relay server
};

#endif