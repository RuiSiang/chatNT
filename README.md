# chatNT
![test-compiling CI](https://github.com/RuiSiang/chatNT/workflows/test-compiling%20CI/badge.svg)

chat Not Tracable: Anonymous p2p chat application in C++ based on onion relay concept.

[Article on Medium](https://ruisiang.medium.com/chatnt-anonymous-p2p-chat-application-in-c-264f52e555b0)

## Supported OS:
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

## Motivation:
In the past few years, freedoms of speech and press has been steeply violated all around the world. chatNT is written and designed to provide journalists and whistleblowers a reliable messaging method without fear of censorship or knowledge of their whereabouts.

## How it Works:

When each peer is initialized, a RSA keypair is generated, with the public key and hashid(hash of the public key) sent to and registered at the server. Let's say peer A wants to send a message to peer B.

0. Peer A routinely fetches the newest user list(ip, port, public key, hashid) of all peers from the relay server and validates the information(rehash the public key to check hashid).
1. Peer A selects 3 random peers from the local user list, say P1, P2, and P3.
2. Peer A forms a specially crafted packet formatted as the following. As you can see, each peer can only decrypt a portion of the packet, hence passing on the packet without knowledge of the full information. As you can see, each peer can only decrypt a portion of the packet, hence passing on the packet without knowledge of the full information.

![packet image](https://github.com/RuiSiang/chatNT/blob/main/images/packet.jpg?raw=true)

3. Peer A sends the packet to P1 via P2P.
4. P1 decrypts his part, gets hashid on P2, looks up information on P2 and sends the packet minus his part to P2 via P2P.
5. P2 decrypts his part, gets hashid on P3, looks up information on P3 and sends the packet minus his part to P3 via P2P.
6. P3 decrypts his part, gets hashid on receipient(B),looks up information on B and sends the inner packet of part P3 to B via P2P.
7. B decrypts the inner packet(part R) and gets the message and sender hashid.

To sum it up, P1 only knows about A and P2, P2 only knows about P1 and P3, P3 only knows about P2 and B, B knows about P3 and A(only hashid, no ip nor port)
## Troubleshoot:
- Getting error "Error opening terminal: xterm" when running peer in Linux. This error is caused by missing environmental variables required by ncurses library. Simply run the shell script "./peer/bin/linux-env.sh" to add the variables.

## Malicious Scenario Handling
Case: Malicious relay server.

Solution: For the first type of attack, the relay server attempts to alter the public key in the user list. Peer validates hashid against public key provided, and since it is nearly impossible to find a public key with the exact hash id, malicious relay server is easily detected. The other type of attack will be manipulating the ip in the user list. It's pointeless since all parts inside the packet is protected by cryptography, the content of intercepted packets are still unreadable.

Case: Middle peers are compromised

Solution: Software based on onion relay concept rely on large number of peers involved in the network, thus reinforcing the randomness and decreases the chance of a designated peer chosen multiple times. Compromisation of a single middle peer is not sufficient to do any damage(that's the point of the concept). For a two-node compromisation scenario, only when the first and last node is both compromised and a time correlation action occurs, will the malicious actor be able to know what path is used. But still, the message content is protected by cryptography.

## Screenshots
![peer image](https://github.com/RuiSiang/chatNT/blob/main/images/peer.jpg?raw=true)

![relay image](https://github.com/RuiSiang/chatNT/blob/main/images/relay.jpg?raw=true)
