#!/bin/bash

# This script looks for NFS servers on the network and trigers a device detected event
# for every server that it finds.

if [[ $1 != "background" ]] ;then
	echo "Backgrounding ..."
	screen -d -m -S "NFSRadar" "$0" "background"
	exit 0
fi

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh

CAT_FILESERVER=164
CAT_MD=8

## Loging function
function Log() {
	echo "$(date) $*" >> /var/log/pluto/NFSScanner.log
}

## Transform decimal-dot netmask in decimal netmask
function netmaskLen() {
        netmask=$1
        len=0
        for i in 1 2 3 4 ;do
                netmaskPart=$(echo $netmask | cut -d'.' -f$i)

                while [ $netmaskPart -gt 0 ] ;do
                        len=$(( len + $netmaskPart % 2 ))
                        netmaskPart=$(( $netmaskPart / 2 ))
                done
        done

        echo $len
}


while : ;do

	if ! [[ "$IntIP" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
		exit 0
	fi
	if ! [[ "$IntNetmask" =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]] ;then
		exit 0
	fi

	## Store in IntNetwork the network address with netmask in decimal format
	NetMaskLen=$(netmaskLen "$IntNetmask")
	if [[ "$NetMaskLen" == 0 ]]; then
		exit 0
	fi
	IntNetwork="$IntIP/$NetMaskLen"


	## Scan the network for nfs servers
	Log "Scanning network $IntNetwork"

	portscan_output=$( nmap -p 2049 -sS $IntNetwork -oG - | grep open | tr ' \t' '#') 
	if [[ $? != "0" ]]; then
		portscan_output=""
	fi
	
	Q="
		SELECT IPaddress FROM Device INNER JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory IN ($CAT_MD, $CAT_FILESERVER) AND IPaddress is not null;
		SELECT IPaddress FROM UnknownDevices WHERE IPaddress is not null;
		SELECT IPaddress FROM PnpQueue WHERE Category='fileserver' AND IPaddress is not null;
	"
	IP_List="_$(RunSQL "$Q" | sed 's/  */_/g')_"
	
	for outputLine in $portscan_output ;do
		sleep 10
		## Get the important info
		serverIP=$(echo $outputLine | tr '#' ' ' | awk '{print $2}')
		serverName=$(echo $outputLine | tr '#' ' ' | awk '{print $3}' | tr -d '()')
		if [[ -z "$serverName" ]] ;then
			serverName=$(nmblookup -A "$serverIP" | awk '($2 == "<00>") && ($4 != "<GROUP>") {print $1}')
		fi
		[[ -z "$serverName" ]] && serverName="$serverIP"
		serverMAC_ARP=$(arp -n "$serverIP" | tail -n +2 | grep ether | head -n +1 | awk '{print $3}')
		[[ -n "$serverMAC_ARP" ]] && serverMAC="$serverMAC_ARP"
		
		Log "Processing $serverIP (mac: $serverMAC name: $serverName)"

		## If the IP is not there already
		if [[ "$IP_List" != *"_${serverIP}_"* ]] && [[ "$serverIP" != "$IntIP" ]] ;then
			## Report it to the router
			/usr/pluto/bin/MessageSend $DCERouter $PK_Device -1001 2 65 56 "fileserver" 52 3 53 2 49 1837 5 "$serverMAC" 28 "$serverIP" 55 "186|$serverName" 54 "\\\\$serverIP"
			Log "Reporting $serverIP [$serverName] to the router"
			
		## If the ip already exists in the database
		else
			Log "$serverIP is already in the database"
		fi 

	done

	## Wait 60 seconds before doing another scan
	sleep 60
done
