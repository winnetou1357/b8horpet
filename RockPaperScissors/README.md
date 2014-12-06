Rock, Paper, Scissors
---------------------

###Abbreviation

RPS or rps

###Description

Implementation of the well-known "rock, paper, scissors" game in which two programs play the game with each other autonomously. The programs connect to each other directly through the internet. The protocol must not enable the programs to cheat in the game.

###Technology

1. For network, we use boost::asio.
2. For command line arguments, we use boost::program_options.
3. We are trying to use C++11 features extensively, whenever they are useful.

###Requirements Specification

The two programs connect to each other given a hostname or IP address and a port range. The programs play the game in a way that they can not cheat each other, while trying to maximize their scores. The game ends whenever one or the other program decides to finish it, but a game can be continued later.

...

###Game protocol

...

Md5/sha+salt kör, utána salt egyeztetés (ld. játékmenet).
Hystory tárolás checksum célra (ld. ellenőrzés).

...

###Cheat defense

This sectiondescribe the technics that the program uses to detect game partners that are cheating.

###Milestones

1. [x] Basic build system that is able to compile the program with boost/asio `include`d.
2. [x] Hostname resolving.
3. [x] Establishing and closing a TCP connection to a remote computer through the internet.
4. [x] Sending and receiving some data over the network.
5. [ ] Implement the protocol of the game (i.e. two programs can play a game with each other).
6. [ ] Implement symmetric network connection method. This means that there is no need to specify a **server** or **client** role for the program at startup, just the remote host name and the port range that can be used for communication.
7. [ ] Implement history handling and other cheat avoiding technics (see Game Protocol).
8. [ ] Implement ability to play with more that one partner at the same time.
