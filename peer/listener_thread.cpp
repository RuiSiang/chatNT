#include "listener_thread.h"
#include "logger.h"

#include <cstring>
#include <thread>
#include <unistd.h>

#define LISTENER_NUM 10

#ifdef __linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#elif _WIN32
#include <WinSock2.h>
#endif

using namespace std;

//constructor
ListenerThread::ListenerThread(int port, SocketControl *_mainsocketControl, SslHandler *_sslHandler, std::vector<User> *_userList, std::vector<Message> *_messages)
{
  //socket initialization
  mainSocketControl = _mainsocketControl;
  sslHandler = _sslHandler;
  userList = _userList;
  messages = _messages;
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

//starts listening for incoming connections
void ListenerThread::startListen()
{
  listen(listenerSocketDescriptor, LISTENER_NUM);
  while (true)
  {
    struct sockaddr_in incomingClientInfo;
    int infoSize = sizeof(incomingClientInfo);

    //accepts incoming connection
    int incomingClientSocketDescriptor = accept(listenerSocketDescriptor, (struct sockaddr *)&incomingClientInfo, &infoSize);
    if (incomingClientSocketDescriptor <= 0)
    {
      break;
    }
    info("Incoming request assigned with descriptor " + to_string(incomingClientSocketDescriptor) + "\n");
    info("(originated from ip: " + string(inet_ntoa(incomingClientInfo.sin_addr)) + ", port: " + to_string(ntohs(incomingClientInfo.sin_port)) + ")\n");

    //creates and assigns a new thread to handle the incoming connection
    HandlerThread *newThread = new HandlerThread(incomingClientSocketDescriptor, mainSocketControl, sslHandler, userList, messages);
    thread sth(&HandlerThread::handler, newThread);
    sth.detach();
  }
  delete this;
}

//destructor
ListenerThread::~ListenerThread()
{
  closesocket(listenerSocketDescriptor);
}
