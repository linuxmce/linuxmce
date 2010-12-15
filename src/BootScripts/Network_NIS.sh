#!/bin/bash

. /usr/pluto/bin/Network_Parameters.sh

#NIS_Domain=$(</etc/defaultdomain)
#if [[ -z "$NIS_Domain" ]]; then
	NIS_Domain=pluto
#fi
echo "Setting NIS, with domain '$NIS_Domain'"
domainname "$NIS_Domain"

IntNetIP=""
for i in 1 2 3 4 ;do
	[[ "$IntNetIP" != "" ]] && IntNetIP="${IntNetIP}."
	maskPart=$(echo $IntNetmask | cut -d'.' -f$i)
	ipPart=$(echo $IntIP | cut -d'.' -f$i)
	IntNetIP="${IntNetIP}$(($ipPart&$maskPart))"
done

if [[ "$ExtIP" == dhcp ]]; then
	ExtIP="$ExtIPreal"
fi

if ! BlacklistConfFiles '/etc/ypserv.securenets' ;then
	if [ ! -e /etc/ypserv.securenets.pbackup ] ;then
		cp /etc/ypserv.securenets /etc/ypserv.securenets.pbackup
	fi
	echo "
	host 127.0.0.1
	host $ExtIP
	$IntNetmask $IntNetIP
	" > /etc/ypserv.securenets
fi
sed -i 's/master|slave|\[Yy\]/slave|[Yy]/g' /etc/init.d/nis

service nis stop
echo | /usr/lib/yp/ypinit -m
service nis start
