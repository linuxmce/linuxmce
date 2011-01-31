#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

# Make sure dhclient writes a few options to /etc/resolv.conf
if [[ -e /etc/dhcp3/dhclient.conf ]] ;then
sed --in-place -e "/^supersede domain-name \"linuxmce.local\"/d" /etc/dhcp3/dhclient.conf
sed --in-place -e "/^prepend domain-name-servers 127\.0\.0\.1/d" /etc/dhcp3/dhclient.conf
echo "prepend domain-name-servers 127.0.0.1;" >> /etc/dhcp3/dhclient.conf
echo "supersede domain-name \"linuxmce.local\";" >> /etc/dhcp3/dhclient.conf
fi

/usr/pluto/bin/Network_Bind.sh
