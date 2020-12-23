// listener_thread.h
#ifndef LISTENER_THREAD_H
#define LISTENER_THREAD_H
#include "socket_control.h"

class ListenerThread
{
public:
    ListenerThread(int, SocketControl *);
    void startListen();

private:
    int listenerSocketDescriptor;
    SocketControl *mainSocketControl;
};

#endif