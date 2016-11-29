#!/bin/sh
# Assume root access for all of this.

# Do not remove libpcap-dev. It might have already been on the system
# so no need to remove it.

# Delete the folder /.keylogger, where the keylogger lives.
sudo rm -r /.keylogger

sudo update-rc.d -f keylogger.sh remove
rm /etc/init.d/keylogger.sh
