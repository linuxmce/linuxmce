#!/bin/bash
# enable Wake-on-LAN on network interfaces

Halt_Script=/etc/default/halt

echo "Enabling Wake-on-LAN (WOL) for network interfaces"
NICS=$(ip -o link | grep "link/ether" | awk '{print $2;}' | cut -d':' -f1)
for interface in $NICS ; do
	ethtool -s $interface wol g
done

#need to add NETDOWN=no to /etc/default/halt
grep -v NETDOWN "$Halt_Script" | sed '/^$/d' | sed '$a\NETDOWN=no' > "$Halt_Script"

