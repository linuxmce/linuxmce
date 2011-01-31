#!/bin/bash
# Configure linux-igd to properly integrate with LMCE.
. /usr/pluto/bin/Network_Parameters.sh
# Do
if [ -n "$IntIf" ] && [ -z "$( echo $IntIf | grep : )"  ] && [ -e /etc/default/linux-igd ] ;then
	sed -i "s/^#INTIFACE/INTIFACE/" /etc/default/linux-igd
	sed -i "s/^#EXTIFACE/EXTIFACE/" /etc/default/linux-igd
	sed -i "s/^INTIFACE=\"*\(.*\)\"*/INTIFACE=${IntIf}/" /etc/default/linux-igd
	sed -i "s/^EXTIFACE=\"*\(.*\)\"*/EXTIFACE=${ExtIf}/" /etc/default/linux-igd
	if [ -n "$( ifconfig -s | grep $IntIf )" ] ;then
		/etc/init.d/linux-igd restart
	fi
fi
