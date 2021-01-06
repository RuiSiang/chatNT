all: peer clean

#variables
CC = g++
INCLUDE = -I. -I./include
CFLAGS = -Wall -Wextra -g -std=c++14
ifdef OS
   PFLAGS = -static-libgcc -static-libstdc++ -lpthread -lws2_32 lib/libssl.lib lib/libcrypto.lib -L${CURDIR}/lib -lncursesw -lformw -lpanelw -lmenuw
else
   ifeq ($(shell uname), Linux)
      PFLAGS = -static-libgcc -static-libstdc++ -L${CURDIR}/lib -lSDL2main -lSDL2 -lssl -lcrypto -lz -ldl -lpthread
   endif
endif

#runner
peer: peer.o listener_thread.o handler_thread.o socket_control.o logger.o ssl_handler.o base64.o peer_control.o ui_utils.o
	$(CC) peer.o listener_thread.o handler_thread.o socket_control.o logger.o ssl_handler.o base64.o peer_control.o ui_utils.o -o bin/peer $(PFLAGS)

peer.o: peer.cpp
	$(CC) $(INCLUDE) $(CFLAGS) -c peer.cpp

listener_thread.o: listener_thread.cpp listener_thread.h
	$(CC) $(INCLUDE) $(CFLAGS) -c listener_thread.cpp

handler_thread.o: handler_thread.cpp handler_thread.h
	$(CC) $(INCLUDE) $(CFLAGS) -c handler_thread.cpp

socket_control.o: socket_control.cpp socket_control.h
	$(CC) $(INCLUDE) $(CFLAGS) -c socket_control.cpp

logger.o: logger.cpp logger.h
	$(CC) $(INCLUDE) $(CFLAGS) -c logger.cpp

ssl_handler.o: ssl_handler.cpp ssl_handler.h
	$(CC) $(INCLUDE) $(CFLAGS) -c ssl_handler.cpp

base64.o: base64.cpp base64.h
	$(CC) $(INCLUDE) $(CFLAGS) -c base64.cpp

peer_control.o: peer_control.cpp peer_control.h
	$(CC) $(INCLUDE) $(CFLAGS) -c peer_control.cpp

ui_utils.o: ui_utils.cpp ui_utils.h
	$(CC) $(INCLUDE) $(CFLAGS) -c ui_utils.cpp

clean:
	rm -f peer.o
	rm -f listener_thread.o
	rm -f handler_thread.o
	rm -f socket_control.o
	rm -f logger.o
	rm -f ssl_handler.o
	rm -f base64.o
	rm -f peer_control.o
	rm -f ui_utils.o
