#!/bin/sh

if [[ -f /tmp/.reset-pluto ]]; then
	exit
fi
touch /tmp/.reset-pluto

mkdir -p /etc/pluto/save/
cp /etc/pluto/* /etc/pluto/save/
cp /etc/pluto/orig/* /etc/pluto/

uci set network.wan.proto=dhcp
uci set network.wan.ifname=eth0.1

Model=$(cat /proc/diag/model)
case "$Model" in
	"ASUS WL-500g Premium")
		uci set network.eth0.vlan0="1 2 3 4 5*"
		uci set network.eth0.vlan1="0 5"
		;;
	"ASUS WL-500g Premium V2")
		uci set network.eth0.vlan0="0 1 2 3 5*"
		uci set network.eth0.vlan1="4 5"
		;;
esac
uci commit

sync
reboot
