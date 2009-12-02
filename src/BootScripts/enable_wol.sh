#!/bin/bash
# enable Wake-on-LAN on eth0

Halt_Script=/etc/default/halt

echo "Enabling Wake-on-LAN (WOL) for eth0"
ethtool -s eth0 wol g

#need to add NETDOWN=no to /etc/default/halt
grep -v NETDOWN "$Halt_Script" | sed '/^$/d' | sed '$a\NETDOWN=no' > "$Halt_Script"

