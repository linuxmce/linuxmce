#!/bin/sh
# Configure Mediatomb to properly integrate with LMCE.
sed -i "/INTERFACE/d" /etc/default/mediatomb
# grep -v "INTERFACE" /etc/default/mediatomb>/etc/default/mediatomb
echo "INTERFACE=\"eth1\"">>/etc/default/mediatomb
usermod -G public -a mediatomb
mysql -e "CREATE DATABASE IF NOT EXISTS pluto_mediatomb"
/etc/init.d/mediatomb restart
