// listener_thread.h
#ifndef LISTENER_THREAD_H
#define LISTENER_THREAD_H

#include <vector>
#include <string>
#include "logger.h"

struct Dataset
{
    std::string hashId;
    std::string ip;
    int port;
    std::string publicKey;
};
class ListenerThread
{
public:
    ListenerThread(int);
    void startListen(int);

private:
    int listenerSocketDescriptor;
    std::vector<Dataset> dataset;
};

#endif
