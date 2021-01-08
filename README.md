# chatNT
![test-compiling CI](https://github.com/RuiSiang/chatNT/workflows/test-compiling%20CI/badge.svg)

chat Not Tracable: Anonymous p2p chatroom application in C++ based on onion relay concept.

## Supported OS
Windows and Linux (tested on windows-latest and ubuntu-latest)

## Dependencies (Compile):
stdc++11, mingw64(windows) or build-essentials(linux), WinSock2.h(windows) or socket.h(linux), thread.h, zlib(linux)

## Libraries:
openssl, SDL2 (both already included in /include and /lib directories)

## Compile:
```
make -C peer
make -C relay
```

## Run:
```
./relay/bin/relay
./peer/bin/peer
```
