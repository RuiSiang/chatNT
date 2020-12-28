// listener_thread.h
#ifndef LISTENER_THREAD_H
#define LISTENER_THREAD_H
#include "socket_control.h"
#include "handler_thread.h"

#include <vector>

//master socket thread to establish and assign parallel threads
class ListenerThread
{
public:
    ListenerThread(int, SocketControl *, SslHandler *, std::vector<User> *, std::vector<Message> *); //constructor
    ~ListenerThread(); //destructor
    void startListen(); //listener process

private:
    int listenerSocketDescriptor; //socket file descriptor for listener thread
    SocketControl *mainSocketControl; //socket file descriptor for connection to relay server
    SslHandler *sslHandler; //ssl object for cryptor operations
    std::vector<User> *userList; //saves user information
    std::vector<Message> *messages; //saves all messages
};

#endif