#!/bin/sh

if [[ -f /tmp/.reset-pluto ]]; then
	exit
fi
touch /tmp/.reset-pluto

mkdir -p /etc/pluto/save/
cp /etc/pluto/* /etc/pluto/save/
cp /etc/httpd.conf /etc/pluto/save/
mv /etc/TZ /etc/TZ-full /etc/pluto/save/
cp /etc/pluto/orig/* /etc/pluto/
cp /etc/pluto.conf /etc/pluto/save/

rm -rf /etc/fiire-ra
rm -f /etc/pluto/ra_password /etc/pluto/ra_ports
rm -f /etc/pluto/pluto.json.lzo.1  /etc/pluto/pluto.json.lzo.2


sed -ir '/^\/(cgi-bin|cisco)\/*/d' /etc/httpd.conf
passwd -d root
sed -i 's/^root:/root:!/' /etc/passwd

uci set network.wan.proto=dhcp
uci set network.wan.ifname=eth0.1

Cfg=$(uci show wireless|grep device=wl0|cut -d. -f2)
uci set wireless.wl0.disabled=1
uci set wireless.wl0.channel=5
uci set wireless.$Cfg.network="lan"
uci set wireless.$Cfg.mode="ap"
uci set wireless.$Cfg.hidden=0
uci set wireless.$Cfg.ssid="HomeControl"
uci set wireless.$Cfg.encryption="none"


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
