#!/bin/sh

Cfg="$1"
CfgMasq="$2"

if [[ -z "$Cfg" ]] || [[ -z "$CfgMasq" ]]; then
	Cfg=$(uci show dhcp|grep interface=lan|cut -d. -f2)
	CfgMasq=$(uci show dhcp|grep =dnsmasq|cut -d. -f2|cut -d= -f1)
fi

echo "$(date -R) Router mode: gateway / begin" >>/tmp/log.routermode

#enable dhcp
		uci set dhcp.$Cfg.ignore=0
		uci set dhcp.$Cfg.dynamicdhcp=1
		uci set dhcp.$Cfg.force=1
		uci set dhcp.$CfgMasq.authoritative=1

#conf vlan's
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

uci set network.wan.ifname=eth0.1

#kill dhcp client on other if
Dhclient=$(ps ax|grep 'udhcpc .*br-lan:0'|grep -v grep|awk '{print $1}')
if [[ -n "$Dhclient" ]]; then
	kill "$Dhclient"
fi

uci commit
sync

#del other if
ifconfig br-lan:0 down 2>/dev/null

#enable firewall 
sed -ir '/^ *accept:.*/d' /etc/firewall.config

#restart services
/etc/init.d/network restart
/etc/init.d/firewall restart
/etc/init.d/dnsmasq restart
echo "$(date -R) Router mode: gateway / end" >>/tmp/log.routermode
