#!/bin/sh
# Configure Mediatomb to properly integrate with LMCE.
echo "INTERFACE=\"eth1\"">>/etc/default/mediatomb
usermod -G public -a mediatomb
DB_EXISTS=0
mysql -e "show databases"|grep -q pluto_mediatomb && DB_EXISTS=1
if [ $DB_EXISTS == 0 ]; then
        mysql -e "CREATE DATABASE pluto_mediatomb;"
fi
/etc/init.d/mediatomb restart
