#!/bin/bash
# Configure linux-igd to properly integrate with LMCE.
. /usr/pluto/bin/Network_Parameters.sh
# Do not configure linux-igd if single-nic system
IntIfUp=$(ip addre | grep inet | grep $IntIf)
if [ -n "$IntIf" ] && [ -z "$( echo $IntIf | grep : )"  ] && [ -e /etc/default/linux-igd ] ;then
	sed -i "s/^#INTIFACE/INTIFACE/" /etc/default/linux-igd
	sed -i "s/^#EXTIFACE/EXTIFACE/" /etc/default/linux-igd
	sed -i "s/^INTIFACE=\"*\(.*\)\"*/INTIFACE=${IntIf}/" /etc/default/linux-igd
	sed -i "s/^EXTIFACE=\"*\(.*\)\"*/EXTIFACE=${ExtIf}/" /etc/default/linux-igd
	# Do not start linux-igd if internel interface is not yet up
	if [ -n "IntIfUp" ] ;then
		/etc/init.d/linux-igd restart
	fi
fi
