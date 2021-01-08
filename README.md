# chatNT
![test-compiling CI](https://github.com/RuiSiang/chatNT/workflows/test-compiling%20CI/badge.svg)

chat Not Tracable: Anonymous p2p chatroom application in C++ based on onion relay concept.

## Supported OS
Windows and Linux (tested on windows-latest and ubuntu-latest)

## Dependencies (Compile):
stdc++11, mingw64(windows) or build-essentials(linux), WinSock2.h(windows) or socket.h(linux), thread.h, zlib(linux)

## Libraries:
openssl, ncursesw (both already included in include and lib directories, no installation needed)

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

## Motivation
In the past few years, freedoms of speech and press has been steeply violated all around the world. chatNT is written and designed to provide journalists and whistleblowers a reliable messaging method without fear of censorship or knowledge of their whereabouts.

## How it Works:
When each peer is initialized, a RSA keypair is generated, with the public key and hashid(hash of the public key) sent to and registered at the server. Let's say peer A wants to send a message to peer B.

0. Peer A routinely fetches the newest user list(ip, port, public key, hashid) of all peers from the relay server and validates the information(rehash the public key to check hashid).
1. Peer A selects 3 random peers from the local user list, say P1, P2, and P3.
2. Peer A forms a specially crafted packet formatted as the following.
![packet image](https://github.com/RuiSiang/chatNT/blob/main/images/packet.jpg?raw=true)
As you can see, each peer can only decrypt a portion of the packet, hence passing on the packet without knowledge of the full information.
3. Peer A sends the packet to P1 via P2P.
4. P1 decrypts his part, gets hashid on P2, looks up information on P2 and sends the packet minus his part to P2 via P2P.
5. P2 decrypts his part, gets hashid on P3, looks up information on P3 and sends the packet minus his part to P3 via P2P.
6. P3 decrypts his part, gets hashid on receipient(B),looks up information on B and sends the inner packet of part P3 to B via P2P.
7. B decrypts the inner packet(part R) and gets the message and sender hashid.
## Troubleshoot:
- Getting error "Error opening terminal: xterm" when running peer in Linux. This error is caused by missing environmental variables required by ncurses library. Simply run the shell script "./peer/bin/linux-env.sh" to add the variables.

## Malicious Scenario Handling
TBD

## Screenshots
![peer image](https://github.com/RuiSiang/chatNT/blob/main/images/peer.jpg?raw=true)

![relay image](https://github.com/RuiSiang/chatNT/blob/main/images/relay.jpg?raw=true)