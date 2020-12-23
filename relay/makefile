all: relay clean

#variables
CC = g++
INCLUDE = .
CFLAGS = -Wall -Wextra -ansi -g -std=c++14
PFLAGS = -O3 -lpthread -lws2_32 #uncomment that for windows compiler

#runner
relay: relay.o listener_thread.o handler_thread.o logger.o
	$(CC) relay.o listener_thread.o handler_thread.o logger.o -o relay $(PFLAGS)

relay.o: relay.cpp listener_thread.h handler_thread.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c relay.cpp

listener_thread.o: listener_thread.cpp listener_thread.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c listener_thread.cpp

handler_thread.o: handler_thread.cpp handler_thread.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c handler_thread.cpp

logger.o: logger.cpp logger.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c logger.cpp

clean:
	rm -f relay.o
	rm -f listener_thread.o
	rm -f handler_thread.o
	rm -f logger.o