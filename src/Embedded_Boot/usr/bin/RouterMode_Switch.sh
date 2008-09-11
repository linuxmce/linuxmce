#!/bin/sh

Cfg="$1"
CfgMasq="$2"

if [[ -z "$Cfg" ]] || [[ -z "$CfgMasq" ]]; then
	Cfg=$(uci show dhcp|grep interface=lan|cut -d. -f2)
	CfgMasq=$(uci show dhcp|grep =dnsmasq|cut -d. -f2|cut -d= -f1)
fi

uci set dhcp.$Cfg.dynamicdhcp=0
uci set dhcp.$CfgMasq.authoritative=0
uci set network.eth0.vlan0="0 1 2 3 4 5*"
uci del network.eth0.vlan1
uci set network.wan.ifname=br-lan:0
ifconfig eth0.1 down

uci commit

/etc/init.d/firewall restart
/etc/init.d/dnsmasq restart
/etc/init.d/network restart
