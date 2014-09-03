Experimental projects with b8horpet
===================================

This repository contains projects (one in each subdirectory) which b8horpet and I use to learn and experiment some tools and technologies that we are currently not familiar with but are interested in.

Rock, Paper, Scissors
---------------------

###Abbreviation

RPS or rps

###Description

Implementation of the well-known "rock, paper, scissors" game in which two programs play the game with each other. The programs connect to each other directly through the internet. The protocol must not enable the programs to cheat in the game.

###Technology

1. For network, we use boost::asio.
2. For command line arguments, we use boost::program_options.
3. We are trying to use C++11 features extensively, whenever they are useful.

###Requirements Specification
...
The two programs connect to each other given a hostname or IP address and a port range. The programs play the game in a way that they can not cheat each other, while trying to maximize their scores. The game ends whenever one or the other program decides to finish it, but a game can be continued later.
...

###Milestones

1. Forduljon az asio include után.
2. Tudjon hostnevet feloldani (távoli gépen is), TCP kapcsolatot létrehozni.
3. Tudjon tetszőleges adatot küldeni a kiépített kapcsolaton.
4. Az adat feleljen meg a játék szabályainak (ld. protokoll).
5. Md5/sha+salt kör, utána salt egyeztetés (ld. játékmenet).
6. Hystory tárolás checksum célra (ld. ellenőrzés).
7. Szimmetrikus hálózati kapcsolódás: egy port range megadása után a két program automatikusan fogad és indít kapcsolódási próbálkozásokat, és az első sikeresen kiépített kapcsolaton folyik a játék.
8. ...
