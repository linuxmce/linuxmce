#!/bin/sh

Cfg="$1"
CfgMasq="$2"

if [[ -z "$Cfg" ]] || [[ -z "$CfgMasq" ]]; then
	Cfg=$(uci show dhcp|grep interface=lan|cut -d. -f2)
	CfgMasq=$(uci show dhcp|grep =dnsmasq|cut -d. -f2|cut -d= -f1)
fi

uci set dhcp.$Cfg.dynamicdhcp=1
uci set dhcp.$CfgMasq.authoritative=1

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

uci commit

/etc/init.d/firewall restart
/etc/init.d/dnsmasq restart
/etc/init.d/network restart
