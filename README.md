# chatNT (chat Not Tracable)
Anonymous p2p chatroom application in C++ based on onion relay concept.

## Supported OS
Windows and Linux (tested on windows-latest and ubuntu-latest)

## Dependencies:
stdc++11, WinSock2.h(windows) or socket.h(linux), thread.h

## Libraries:
openssl, SDL2 (both already included in /include and /lib directories)

## Compile:
```
make -C peer
make -C relay
```