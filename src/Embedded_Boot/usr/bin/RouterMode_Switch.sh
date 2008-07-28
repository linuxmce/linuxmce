#!/bin/sh

Cfg="$1"
CfgMasq="$2"

if [[ -z "$Cfg" ]] || [[ -z "$CfgMasq" ]]; then
	Cfg=$(uci show dhcp|grep interface=lan|cut -d. -f2)
	CfgMasq=$(uci show dhcp|grep =dnsmasq|cut -d. -f2|cut -d= -f1)
fi

echo "$(date -R) Router mode: switch / begin" >>/tmp/log.routermode

#disable dhcpd
		uci set dhcp.$Cfg.ignore=0
		uci set dhcp.$Cfg.force=1
		uci set dhcp.$Cfg.dynamicdhcp=0
		uci set dhcp.$CfgMasq.authoritative=0

#conf vlan's
uci set network.eth0.vlan0="0 1 2 3 4 5*"
uci del network.eth0.vlan1

uci set network.wan.ifname=br-lan:0

#kill dhcp client on other if
Dhclient=$(ps ax|grep 'udhcpc .*eth0.1'|grep -v grep|awk '{print $1}')
if [[ -n "$Dhclient" ]]; then
	kill "$Dhclient"
fi

uci commit
sync

#del other if
ifconfig eth0.1 down

/etc/init.d/network restart
/etc/init.d/firewall restart
/etc/init.d/dnsmasq restart
echo "$(date -R) Router mode: switch / end" >>/tmp/log.routermode
