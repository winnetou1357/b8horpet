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

###Requirements

* The two programs connect to each other given a hostname or IP address and a port range to scan for the peer.
* The programs play the game in a way that they can not cheat each other, while trying to maximize their scores.
** More specifically the communication protocol should be implemented so that it reveals an attempt to cheating immediately. If one of the clients detects that its oppenent is cheating, it should end the game and optionally remember not to play with the same peer again.
* The game ends whenever one or the other program decides to finish it. A client can not end the game interrupting rounds but only between them, and the program should notify its peer on its willing to end the game. Optionally a game can be continued later with the same peer, if both can verify the identity of the other.

###Game protocol

This protocol describes the communication between the two players (peers). It aims to be specified so that:
* it is eligible for playing the game and for manage all other communications needed to fulfill the requirements (such as end-of-game notification),
* it prevents cheating in the way that the peer can not take advantage of the ordering of packages, i.e. that a packages containing the choice of the peer can arrive before the other figures out its choice,
* it reveals cheating by modifying the overall scores immediately: the peers must agree in the state of the game after each round.

This protocol does not describe how to initiate, build up and keep alive metwork communication but assumes that a trusted and reliable communication channel has been established between the peers before.

####General properties

One general property of the protocol is that is is symmetric in the way that every step of communication is done and acknowledged by both peers before they can advance to the next step of the communication.

1. Initiate communication: in this phase the peers exchange some unique ID or name of them, probably a GUID. Its aim is a simple identification so that game session could be resumed after a lost connection or sg like that.

2. Initiate round: both peers send a string "end" if they want to end the game or "play" if they want to continue playing.

3. Start of round: both peers send their choice to each other in the following format: md5(<salt><choice><salt>) where <salt> is a nonempty random-generated value that is unique for that round and <choice> is one of the characters 'R' 'P' or 'S' for 'rock', 'paper' and 'scissors' respectively.

4. Decrypt choices: after the encrypted choices arrived, both peers send the salt it used. The choice of the peer can be decrypted by trying the 3 possible values 'R', 'P' and 'S' encrypt with the salt received, and whichever matches the encrypted choice received before gives the actual choice of the peer.

5. Aggree in the state of game: both peers send its interpretation of the state of the game in the form of a checksum. A state of one round is given by 2 characters: the choice of the current peer and the choice of its opponent, respectively, e.g. "RS". Te state of the game is a string of such two-char sequences separated by newlines ('\n' characters), in order of time. An MD5 checksum of this state is generated and sent to the peer.

6. Go to 2.

In any step of communication a peer can send 'cheater' and end further communication if it thinks that the other side cheats.

In any communication step an error can occur and the game ends, not counting an eventual unfinished round.

###Issues and points to consider

* Its hard to define a timeout for the 4. point of the game protocol, namely the timeout acceptable between sending encrypted choices and the salt to decrypt them. If the timeout is too long one can break the MD5 hash regardless of the salt and take advantage of message ordering. A too short timeout along with a high network latency can lead to end of the game with no real reason.
