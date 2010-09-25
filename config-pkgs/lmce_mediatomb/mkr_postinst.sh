#!/bin/bash
# Configure Mediatomb to properly integrate with LMCE.
. /usr/pluto/bin/Network_Parameters.sh
sed -i "s/^INTERFACE=\"*.*\"*/INTERFACE=\"${IntIf}\"/" /etc/default/mediatomb
usermod -G public -a mediatomb
mysql -e "CREATE DATABASE IF NOT EXISTS pluto_mediatomb"
route del -net 239.0.0.0 netmask 255.0.0.0
/etc/init.d/mediatomb stop
/etc/init.d/mediatomb start
