// listener_thread.h
#ifndef LISTENER_THREAD_H
#define LISTENER_THREAD_H
#include "socket_control.h"
#include "handler_thread.h"

#include <vector>

class ListenerThread
{
public:
    ListenerThread(int, SocketControl *, SslHandler *, std::vector<User> *, std::vector<Message> *);
    ~ListenerThread();
    void startListen();

private:
    int listenerSocketDescriptor;
    SocketControl *mainSocketControl;
    SslHandler *sslHandler;
    std::vector<User> *userList;
    std::vector<Message> *messages;
};

#endif