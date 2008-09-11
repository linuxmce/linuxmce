#!/bin/sh

# Bridged mode
if [[ "`uci get network.eth0.vlan1`" == '' ]] ;then
	# With Static IP
	if [[ "`uci get network.lan.proto`" == 'static' ]] ;then
		Report_IP=`uci get network.lan.ipaddr`

	# With Dynamic IP
	else
		wait=0
		while [[ "$Report_IP" == "" ]] && [[ "$wait" != "15" ]] ;do
			Report_IP=`ifconfig br-lan | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g'`
			sleep 1
			wait=$(( wait + 1 ))
		done
	fi

# Routing mode
else
 	# With Static IP
	if [[ "`uci get network.wan.proto`" == 'static' ]] ;then
		Report_IP=`uci get network.wan.ipaddr`
	
	# With Dynamic IP
	else
		wait=0
		while [[ "$Report_IP" == "" ]] && [[ "$wait" != "15" ]] ;do
			Report_IP=`ifconfig eth0.1 | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g'`
			sleep 1
			wait=$(( wait + 1 ))
		done
	fi
fi

Report_MAC=`ifconfig eth0 | grep 'HWaddr' | sed 's/.*HWaddr \([0-9A-F:]*\).*/\1/g'`


echo "Reporting $Report_IP - $Report_MAC"
wget -qO/dev/null "http://plutohome.com/ReportAP.php?IP=${Report_IP}&MAC=${Report_MAC}"
