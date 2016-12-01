# Keylogger
This is a keylogger and network sniffer created for malware purposes for the Final Project of the Systems II course at OSU during Autumn 16. This malware consists of a client-side program and a server-side program. The client-side program is to be installed on the victim's computer and will then automatically run on startup. The client-side program has two processes: `main` and `ghost`. `main` does all the network sniffing and keylogging, and ghost is there simply to keep main running. If the victim discovers main and kills it, ghost will start it up, and vice versa. Only on identifying both processes can the victim completely stop the malware. 

## Install
To install the keylogger there is a bash script, `install.sh` which prompts to install the needed dependencies, `make`s the source, and moves the startup script, `keylogger.sh` to `/etc/init.d/` and registers it with `update-rc.d`. Further, the startup script creates the program in `/.keylogger`. Run the following command to install:

```
sudo ./install.sh
```

## Uninstall
To uninstall the keylogger, run the bash script, `uninstall.sh` which deletes the folder `/.keylogger` and removes the startup script from `/etc/init./d` and removes it with `update-rc.d`. Run the following command to uninstall:

```
sudo ./uninstall.sh
```

## Use
After running the install script, the keylogger should start automatically on each subsequent bootup of the system. Note that if `./server` is not run before hand, the keylogger, will not reconnect later. Therefore, if you wisth to capture the victim's data, make sure there is a server program running before the victim starts their computer. If you are testing on your system, then `ps -aux` to kill the processes, ghost and main that started on boot, and manually run the program. Run the following commands from the source directory:

```
make server
./server

cd /.keylogger
sudo ./main
```

### Notes
 - Make sure before hand that the client.c code has the ip address you want to send the information to, which will be where the server is running.
 - server will save two log files every three minutes in the folder in which it is running called keylog_received.txt and network_received.txt.

## Notes
This has only been tested with Ubuntu 16.04.

-- Matias Grioni and John Siracusa
