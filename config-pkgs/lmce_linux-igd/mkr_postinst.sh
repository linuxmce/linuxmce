#!/bin/bash
# Configure linux-igd to properly integrate with LMCE.
. /usr/pluto/bin/Network_Parameters.sh
# Do not configure linux-igd if single-nic system
if [ -n "$IntIf" ] && [ -z "$( echo $IntIf | grep : )"  ] && [ -e /etc/default/linux-igd ] ;then
	sed -i "s/^#INTIFACE/INTIFACE/" /etc/default/linux-igd
	sed -i "s/^#EXTIFACE/EXTIFACE/" /etc/default/linux-igd
	sed -i "s/^INTIFACE=\"*\(.*\)\"*/INTIFACE=${IntIf}/" /etc/default/linux-igd
	sed -i "s/^EXTIFACE=\"*\(.*\)\"*/EXTIFACE=${ExtIf}/" /etc/default/linux-igd
	# Do not start linux-igd if internel interface is not yet configured
	IntIfIP=$(ip addr | grep inet | grep $IntIf | awk '{print $2}' | sed 's,/.*,,g' )
	if [ "IntIfIP" = "$IntIP"  ] ;then
		/etc/init.d/linux-igd restart
	fi
fi
