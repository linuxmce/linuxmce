#!/bin/bash

RAServer="www.linuxmce.org"
#PlutoHome="10.0.0.175/plutohome-com"

. /usr/pluto/bin/Config_Ops.sh

if [[ -z "$PK_Installation" || "$PK_Installation" == 1 ]]; then
	echo "Unusable installation number: '$PK_Installation'"
	exit 1
fi

if [[ -z "$remote" ]]; then
	echo "Remote assistance is not enabled. Nothing to keep alive"
	exit 0
fi

PortPage="http://$RAServer/get_ra_ports.php?installationID=$PK_Installation"
wget -O /dev/null "$PortPage" &>/dev/null
