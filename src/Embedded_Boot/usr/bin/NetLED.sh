#!/bin/sh

if [[ -f /tmp/netled.pid ]]; then
	exit
fi

echo $$ >/tmp/netled.pid

trap 'Cleanup' EXIT
trap 'exit' INT

Cleanup()
{
	if [[ -n "$PID_NoNet" ]]; then
		kill "$PID_NoNet"
	fi
	echo 1 >/proc/diag/led/power
	rm -f /tmp/netled.pid
	echo "$(date -R) NetLED terminated" >>/tmp/log.NetLED
}

LED_NoNet()
{
	Start=$(date +%s)
	Once=false
	while :; do
		for i in 0 1 0 1; do
			echo "$i" >/proc/diag/led/power
			Sleep 100
		done
		Sleep 500
		Now=$(date +%s)
		Difference=$(expr $Now - $Start)
		if [[ -f /etc/pluto/fresh_install ]] && [[ "$Once" == false ]] && [[ "$Difference" -ge 60 ]]; then
			echo "$(date -R) No Internet for 60 seconds. Switching to gateway mode" >>/tmp/log.NetLED
			/usr/bin/RouterMode_Gateway.sh
			Once=true
		fi
	done
}

NoInternet()
{
	if [[ -z "$PID_NoNet" ]]; then
		LED_NoNet &
		PID_NoNet=$!
	fi
}

Keys="google youtube yahoo plutohome"

Srv_google=google.com
Srv_youtube=youtube.com
Srv_yahoo=yahoo.com
Srv_plutohome=plutohome.com

PID_NoNet=

echo "$(date -R) NetLED active" >>/tmp/log.NetLED

while :; do
	Reach=0
	for key in $Keys; do
		eval "server=\$(echo \$Srv_$key)"
		IP=$(/usr/bin/NetworkTest $server 80 2>/dev/null)
		Reachable=$?
		eval "IP_$key=$IP"
		if [[ "$Reachable" -eq 0 ]]; then
			Reach=$(expr $Reach + 1)
		fi
	done

	if [[ "$Reach" -eq 0 ]]; then
		echo "$(date -R) No hosts reachable. No Internet." >>/tmp/log.NetLED
		NoInternet
		sleep 5
		continue
	fi

	LastIP=
	for key in $Keys; do
		eval "IP=\$(echo \$IP_$key)"
		if [[ "$IP" == "$LastIP" ]]; then
			echo "$(date -R) Hosts return same IP address. No Internet." >>/tmp/log.NetLED
			NoInternet
			sleep 5
			continue 2
		fi
		LastIP="$IP"
	done
	echo "$(date -R) Internet connection active. Exiting loop." >>/tmp/log.NetLED
	break
done

/usr/bin/Report_AP.sh &
if [[ ! -f /etc/pluto/fresh_install ]]; then
	echo "$(date -R) Automatic configuration disabled. Exiting." >>/tmp/log.NetLED
	exit
fi
echo "$(date -R) Automatic configuration enabled. Proceeding" >>/tmp/log.NetLED

IPtype=routable

if ifconfig | grep -q br-lan:0; then
	WANip=$(ifconfig br-lan:0 | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g')
else
	WANip=$(ifconfig eth0.1 | grep 'inet addr:' | sed 's/.*inet addr:\([0-9.]*\).*/\1/g')
fi

d1=$(echo "$WANip" | cut -d. -f1)
d2=$(echo "$WANip" | cut -d. -f2)

if [[ "$d1" == 10 ]]; then
	IPtype=local
elif [[ "$d1" == 192 ]] && [[ "$d2" == 168 ]]; then
	IPtype=local
elif [[ "$d1" == 172 ]] && [[ "$d2" -ge 16 ]] && [[ "$d2" -le 31 ]]; then
	IPtype=local
fi

Cfg=$(uci show dhcp|grep interface=lan|cut -d. -f2)
CfgMasq=$(uci show dhcp|grep =dnsmasq|cut -d. -f2|cut -d= -f1)

uci commit
sync

case "$IPtype" in
	local)
		echo "$(date -R) WAN IP is local. Configuring as switch" >>/tmp/log.NetLED
		/usr/bin/RouterMode_Switch.sh "$Cfg" "$CfgMasq"
		;;
	routable)
		echo "$(date -R) WAN IP is routable. Configuring as gateway" >>/tmp/log.NetLED
		/usr/bin/RouterMode_Gateway.sh "$Cfg" "$CfgMasq"
		;;
esac
