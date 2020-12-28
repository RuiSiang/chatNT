// listener_thread.h
#ifndef LISTENER_THREAD_H
#define LISTENER_THREAD_H

#include <vector>
#include <string>
#include "logger.h"

//structure to save user information
struct User
{
    std::string hashId;
    std::string ip;
    int port;
    std::string publicKey;
};

//master socket thread to establish and assign parallel threads
class ListenerThread
{
public:
    ListenerThread(int); //constructor
    void startListen(int); //start listening for incoming connections

private:
    int listenerSocketDescriptor; //socket file descriptor for master listener
    std::vector<User> user; //saves user list
};

#endif
