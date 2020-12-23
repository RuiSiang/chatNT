// handler_thread.h
#ifndef HANDLER_THREAD_H
#define HANDLER_THREAD_H

#include <string>
#include "socket_control.h"

class HandlerThread
{
public:
  HandlerThread(int descriptor, SocketControl *_mainsocketControl)
  {
    threadSocketDescriptor = descriptor;
    mainSocketControl = _mainsocketControl;
  }
  void handler();

private:
  int threadSocketDescriptor;
  SocketControl *mainSocketControl;
  int process(std::string);
};

#endif