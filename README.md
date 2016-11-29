# Keylogger
This is a keylogger and network sniffer created for malware purposes for the Final Project of the Systems II course at OSU during Autumn 16. This malware consists of a client-side program and a server-side program. The client-side program is to be installed on the victim's computer and will then automatically run on startup. The client-side program has two processes: `main` and `ghost`. `main` does all the network sniffing and keylogging, and ghost is there simply to keep main running. If the victim discovers main and kills it, ghost will start it up, and vice versa. Only on identifying both processes can the victim completely stop the malware. 

## Install
To install the keylogger there is a bash script, `install.sh` which prompts to install the needed dependencies, `make`s the source, and moves the startup script, `keylogger.sh` to `/etc/init.d/` and registers it with `update-rc.d`. Further, the startup script creates the program in `/.keylogger`.

## Uninstall
To uninstall the keylogger, run the bash script, `uninstall.sh` which deletes the folder `/.keylogger` and removes the startup script from `/etc/init./d` and removes it with `update-rc.d`.

## Use
After running the install script, the program should start automatically and should start automatically on each subsequent bootup of the system. If this is not the case, you can still run the program manually through these commands:

```
cd /.keylogger
sudo ./main
```

Further, to actually get results from the compromised system, the hacker must have the server running. The hacker does not need to run the install script on their system. Simply run:

```
make server
./server
```

Simply make sure before hand that the client code has the ip address you want to send the information to, which will be where the server is run.

## Notes
This has only been tested with Ubuntu 16.04.

-- Matias Grioni and John Siracusa
