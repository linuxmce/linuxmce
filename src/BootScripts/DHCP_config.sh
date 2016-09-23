#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Network_Parameters.sh
. /usr/pluto/bin/Utils.sh

function refresh_all {
	Q="SELECT PK_Device, IPaddress from Device WHERE IPaddress != ''"
	R=$(RunSQL "$Q")

	for Device in $R ;do
		Device_Id=$(Field "1" "$Device")
		Device_IP=$(Field "2" "$Device")

		RunSQL "UPDATE Device SET IPaddress='' WHERE PK_Device = $Device_Id"
		/usr/pluto/bin/PlutoDHCP -d "$Device_Id" -a
	done

}

WaitLock "PlutoDHCP" "PlutoDHCP_config"
if [[ -z "$DHCPsetting" ]]; then
	rm -f /etc/dhcp/dhcpd.conf
	service isc-dhcp-server stop
	exit
fi

if [[ "$1" == "refresh_all" ]] ;then
	refresh_all
fi

if ! BlacklistConfFiles '/etc/dhcp/dhcpd.conf' ;then
	if [ ! -e /etc/dhcp/dhcpd.conf.pbackup ] ;then
		cp /etc/dhcp/dhcpd.conf /etc/dhcp/dhcpd.conf.pbackup
	fi
	/usr/pluto/bin/PlutoDHCP >/etc/dhcp/dhcpd.conf
	touch /etc/dhcp/dhcpd-extra.conf
	service isc-dhcp-server stop
	service isc-dhcp-server start
fi
Unlock "PlutoDHCP" "PlutoDHCP_config"
