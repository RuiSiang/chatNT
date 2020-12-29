#include "listener_thread.h"
#include "handler_thread.h"

#include <cstring>
#include <thread>
#include <unistd.h>
#include <sys/types.h>

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#elif _WIN32
#include <WinSock2.h>
typedef int socklen_t;
#endif

using namespace std;

//constructor
ListenerThread::ListenerThread(int port)
{
  //socket initialization
  listenerSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (listenerSocketDescriptor == -1)
  {
    error("Listener Socket Creation Failed\n");
    exit(-1);
  }
  else
  {
    info("Listener Established\n");
  }

  //connection initialization
  struct sockaddr_in listenerInfo;
  memset(&listenerInfo, 0, sizeof(listenerInfo));
  listenerInfo.sin_family = PF_INET;
  listenerInfo.sin_addr.s_addr = INADDR_ANY;
  listenerInfo.sin_port = htons(port);
  int err = bind(listenerSocketDescriptor, (struct sockaddr *)&listenerInfo, sizeof(listenerInfo));
  if (err == -1)
  {
    error("Bind port failed, please try again\n");
    exit(-1);
  }
  else
  {
    info("Binded on port " + to_string(port) + "\n");
  }
}

//starts listening for incoming conenctions
void ListenerThread::startListen(int listenerNum)
{
  listen(listenerSocketDescriptor, listenerNum);
  user.clear();
  while (true)
  {
    //accept incoming connection
    struct sockaddr_in incomingClientInfo;
    socklen_t infoSize = sizeof(incomingClientInfo);
    int incomingClientSocketDescriptor = accept(listenerSocketDescriptor, (struct sockaddr *)&incomingClientInfo, &infoSize);
    if (incomingClientSocketDescriptor <= 0)
    {
      break;
    }

    //assign handler thread to incoming connection
    info("Incoming request assigned with thread descriptor " + to_string(incomingClientSocketDescriptor) + " (originated from ip: " + inet_ntoa(incomingClientInfo.sin_addr) + ", port: " + to_string(ntohs(incomingClientInfo.sin_port)) + ")\n");
    HandlerThread *newThread = new HandlerThread(incomingClientSocketDescriptor, &user, string(inet_ntoa(incomingClientInfo.sin_addr)));
    thread sth(&HandlerThread::handler, newThread);
    sth.detach();    
  }

  close(listenerSocketDescriptor);
}
