#!/bin/bash

USAGE="Usage: `basename $0` enable|disable username"

secrets=/etc/ppp/pap-secrets

enable()
{
	username=$(echo $1 | tr '[:upper:]' '[:lower:]')
	echo "Enabling VPN for user $username"
	
	# check if user does not already have VPN access
	grep -w ^$username $secrets >/dev/null
	if [[ "$?" -ne "0" ]]; then
		# check if user maybe exists but is commented out
		grep -w ^#$username $secrets >/dev/null
		if [[ "$?" -eq "0" ]]; then
			echo "$username already exists in secrets file, enabling access"
			sed -i "s/^#$username/$username/" $secrets
		else
			echo "$username does not exist, creating new access"
			echo "$username	*	\"\"	*" >>$secrets
		fi
	else
		echo "$username already has VPN access, nothing to do"
	fi

	#Update DB
	Q=$(mysql pluto_main -ss -e "select * from Firewall where SourceIP='$2';")
	if [[ "$Q" == "" ]]; then
		mysql pluto_main -ss -e "insert into Firewall (RuleType,SourceIP,DestinationIP,RPolicy,Description,Protocol,Offline) VALUES('VPN','$2', '$coreip', 'ACCEPT','Allow VPN User $1 with IP $2','ip-ipv4','1')"
	fi
}

disable()
{
	username=$(echo $1 | tr '[:upper:]' '[:lower:]')
	echo "Disabling VPN for user $username"

	# check if user really has VPN access
	grep -w ^$username $secrets >/dev/null
	if [[ "$?" -eq "0" ]]; then
		echo "$username has enabled VPN access, disabling access"
		sed -i "s/^$username/#$username/" $secrets
	else
		echo "$username does not have VPN access enabled, nothing to do"
	fi

	#Update DB
	if [[ ! "$2" == "" ]]; then
		Q=$(mysql pluto_main -ss -e "select * from Firewall where SourceIP='$2';")
		if [[ ! "$Q" == "" ]]; then
			mysql pluto_main -ss -e "delete from Firewall WHERE SourceIP = '$2'"
		fi
	fi
}
delete()
{
	username=$(echo $1 | tr '[:upper:]' '[:lower:]')
	echo "Deletinging user $username"

	# check if user really has VPN access
	grep -w $username $secrets >/dev/null
	if [[ "$?" -eq "0" ]]; then
		echo "$username does exist in file, deleteing"
		sed -i "/$username/d" $secrets
	else
		echo "$username does not exist in file, nothing to do"
	fi
	
	#
}
change()
{
	username=$(echo $1 | tr '[:upper:]' '[:lower:]')
	if [[ "$2" == "Auto" ]]; then
		sed -i "s/^$username.*/$username  *       \"\"    */" $secrets
	else
		sed -i "s/^$username.*/$username  *       \"\"    $2	#$3/" $secrets
	fi
	case "$3" in
		"Core")
			coreip=$(mysql pluto_main -ss -e "select IPaddress from Device where FK_DeviceTemplate='7';")
			mysql pluto_main -ss -e "UPDATE Firewall SET DestinationIP='$coreip' WHERE RuleType='VPN' AND SourceIP='$2' AND Protocol='ip-ipv4' "
			;;
		"LAN")
			lan=$(mysql pluto_main -ss -e "select IPaddress from Device where FK_DeviceTemplate='7';" | cut -d'.' -f1-3)
			lan=$lan".0/24"
			mysql pluto_main -ss -e "UPDATE Firewall SET DestinationIP='$lan' WHERE RuleType='VPN' AND SourceIP='$2' AND Protocol='ip-ipv4' "
			;;
		"All")
			mysql pluto_main -ss -e "UPDATE Firewall SET DestinationIP='0.0.0.0/0' WHERE RuleType='VPN' AND SourceIP='$2' AND Protocol='ip-ipv4' "
			;;
		*)
			;;
	esac

	#Run Firewall Scripts
	/usr/pluto/bin/Network_Firewall.sh
}

case "$1" in
	"enable")
		enable $2 $3;
		;;
	"disable")
		disable $2 $3;
		;;
	"delete")
		delete $2;
		;;
	"change")
		change $2 $3 $4;
		;;
	*)
		echo $USAGE >&2
		exit 1
		;;
esac
