#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh > /dev/null 2>/dev/null || exit 1
. /usr/pluto/bin/Network_Parameters.sh > /dev/null 2>/dev/null || exit 1
. /usr/pluto/bin/Utils.sh

function DisplayUsage() {
	echo
	echo " TO SET CONFIGURATION DATA:"
	echo " --------------------------"
	echo "  DynamicDNS.sh set <login> <password> <service_params> <use> <domains>"
	echo 
	echo "    login          = login for the dynamic dns service provider"
	echo "    password       = password for the dynamic dns service provider"
	echo "    service_params = dynamic dns service params (see list)"
	echo "    use            = Use web to determine IP address, else get it locally"
	echo "    domains        = comma separated list of domains to update"
	echo
	echo
	echo " TO GET CONFIGURATION DATA:"
	echo " --------------------------"
	echo "  DynamicDNS.sh get"
	echo
	echo "    Will display the 'login', 'service_params' and 'domains' on separate lines"
	echo 
	echo
	echo " LIST OF service_params FOR KNOWN DYNAMIC DNS PROVIDERS:"
	echo " ---------------------------"
	echo "  DynDNS (dynamic)                 : protocol=dyndns2,server=members.dyndns.org"
	echo "  DynDNS (static)                  : protocol=dyndns2,server=members.dyndns.org,static=yes"
	echo "  DynDNS (custom)                  : protocol=dyndns2,server=members.dyndns.org,custom=yes"
	echo "  ZoneEdit (zoneedit.com)          : protocol=zoneedit1,server=www.zoneedit.com"
	echo "  EasyDNS (easydns.com)            : protocol=easydns,server=members.easydns.com"
	echo "  OrgDNS.org account-configuration : protocol=dyndns2,server=www.orgdns.org,use=web,web=members.orgdns.org/nic/ip"
	echo "  DnsPark.com                      : protocol=dnspark,use=web,web=ipdetect.dnspark.com,web-skip='Current Address:'"
	echo "  NameCheap (namecheap.com)        : protocol=namecheap,server=dynamicdns.park-your-domain.com"
	echo "  No-IP (no-ip.com)				 : protocol=noip,server=dynupdate.no-ip.com/nic/update"
	echo 

}

## Backup initial conf files first
if [ ! -e /etc/ddclient.conf.pbackup ]; then
	cp /etc/ddclient.conf /etc/ddclient.conf.pbackup
fi
if [ -e /etc/default/ddclient ] && [ ! -e /etc/default/ddclient.pbackup ] ;then
	cp /etc/default/ddclient /etc/default/ddclient.pbackup
fi

## Called to set the dynamic dns settings
if [[ "$1" == "set" ]] ;then
	shift

	if [[ "$#" != 5 ]] ;then
		DisplayUsage
	fi

	DD_LOGIN="$1"
	DD_PASS="$2"
	DD_SERVICE="$3"
	DD_USE="$4"
	DD_DOMAINS="$5"
	DD_ETH="$ExtIf"

	Vars="DD_LOGIN DD_PASS DD_SERVICE DD_USE DD_DOMAINS DD_ETH"
	if ! BlacklistConfFiles '/etc/ddclient.conf' ;then
		cp /usr/pluto/templates/ddclient.conf.template /etc/ddclient.conf.$$
		ReplaceVars /etc/ddclient.conf.$$
		mv /etc/ddclient.conf.$$ /etc/ddclient.conf
	fi

	if ! BlacklistConfFiles '/etc/default/ddclient' ;then
		echo > /etc/default/ddclient
		echo 'run_ipup="true"' >> /etc/default/ddclient
		echo 'run_daemon="true"' >> /etc/default/ddclient
		echo 'daemon_interval="300"' >> /etc/default/ddclient
	fi

	service ddclient restart

## Called to display the dynamic dns settings
elif [[ "$1" == "get" ]] ;then

	## The file exists
	if [[ -f /etc/ddclient.conf ]] ;then
		## And we where the one that created it
		if cat /etc/ddclient.conf | grep -q -m 1 '^# Pluto - ddclient.conf' ;then
			DD_LOGIN=$(cat /etc/ddclient.conf | grep -m 1 '^ *login=' | cut -d'=' -f2)
			DD_PASS=$(cat /etc/ddclient.conf | grep -m 1 '^ *password=' | cut -d'=' -f2)
			DD_SERVICE=$(cat /etc/ddclient.conf | grep -m 1 '^ *protocol=')
			DD_USE=$(cat /etc/ddclient.conf | grep -m 1 '^ *use=' | cut -d'=' -f2)
			DD_DOMAINS=$(cat /etc/ddclient.conf | egrep -m 1 -v '^ *#|^ *$|.*=.*')
		fi
	fi

	echo $DD_LOGIN
	echo $DD_SERVICE
	echo $DD_USE
	echo $DD_DOMAINS

## Unkown call
else
	DisplayUsage	
fi
