#!/bin/bash
# Configure Mediatomb to properly integrate with LMCE.
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh

sed -i "s/^INTERFACE=\"*.*\"*/INTERFACE=\"${IntIf}\"/" /etc/default/mediatomb

# add mediatomb user to group public
usermod -G public -a mediatomb

# Create database
Q="CREATE DATABASE IF NOT EXISTS pluto_mediatomb"
RunSQL "$Q"

## TODO: WHAT IS THIS DOING HERE?
#route del -net 239.0.0.0 netmask 255.0.0.0 || /bin/true

[[ ! -f /etc/mediatomb/config.xml ]] && cp /etc/mediatomb/config.xml.dpkg-dist /etc/mediatomb/config.xml || :

# ensure the config file is owned by the mediatomb user
chown mediatomb:mediatomb /etc/mediatomb/config.xml || :

# restart the service
service mediatomb stop || :
service mediatomb start
