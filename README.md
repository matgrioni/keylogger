# Keylogger
This is a keylogger and network sniffer created for malware purposes for the Final Project of the Systems II course at OSU during Autumn 16. This malware consists of a client-side program and a server-side program. The client-side program is to be installed on the victim's computer and will then automatically run on startup. The client-side program has two processes: `main` and `ghost`. `main` does all the network sniffing and keylogging, and ghost is there simply to keep main running. If the victim discovers main and kills it, ghost will start it up, and vice versa. Only on identifying both processes can the victim completely stop the malware. 

## Installation
To install the keylogger there is a bash script, `install.sh` which prompts to install the needed dependencies, `make`s the source, and moves the startup script to `/etc/init.d/`. Further, the startup script assumes that the program is in `~/malware/`.

## Notes
This has only been tested with Ubuntu 16.04.

-- Matias Grioni and John Siracusa
