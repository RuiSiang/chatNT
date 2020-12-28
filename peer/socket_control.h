// handler_thread.h
#ifndef SOCKET_CONTROL_H
#define SOCKET_CONTROL_H

#include <string>

//socket controller for active socket connections
class SocketControl
{
public:
  SocketControl(); //constructor
  ~SocketControl(); //destructor
  int bind(char[], int); //bind socket to ip, port
  std::string sendCommand(std::string); //send command with socket

private:
  int socketDescriptor; //socket file descriptor
};
bool CheckPortTCP(short int); //test if port is in use

#endif