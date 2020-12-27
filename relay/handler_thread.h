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
  HandlerThread(int descriptor, std::vector<Dataset> *_dataset, std::string _ip)
  {
    threadSocketDescriptor = descriptor;
    dataset = _dataset;
    ip = _ip;
  }
  void handler();

private:
  int threadSocketDescriptor;
  int process(std::string);
  std::vector<Dataset> *dataset;
  std::string ip;
  std::string hashId;
  void sendMessage(std::string);
  std::string receiveMessage(void);
};

#endif
