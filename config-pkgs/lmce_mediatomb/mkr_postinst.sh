#!/bin/sh
# Configure Mediatomb to properly integrate with LMCE.
echo "INTERFACE=\"eth1\"">>/etc/default/mediatomb
usermod -G public -a mediatomb
echo "CREATE DATABASE pluto_mediatomb;"|mysql
/etc/init.d/mediatomb restart

