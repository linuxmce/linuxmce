#!/bin/sh

#if allready running exit
if [[ -f /tmp/.ezsetup-pluto ]]; then
	exit
fi
touch /tmp/.ezsetup-pluto


###disable wired ports###
#robocfg switch disable

#get wifi settings
uci export wireless > /tmp/wireless.orig

###turn on wi-fi###
uci set wireless.wl0.disabled="0"
Cfg=$(uci show wireless|grep device=wl0|cut -d. -f2)
#set channel to 5
uci set wireless.wl0.channel="5"
#disable broadcast ssid
uci set wireless.$Cfg.hidden="1"
#set ssid
uci set wireless.$Cfg.ssid="fiireezsetup"
#set encryption to none
uci set wireless.$Cfg.encryption="none"
#set wifi ip
ifconfig wl0 10.253.80.1 netmask 255.255.255.0 up
#start wifi
/sbin/wifi up

#start ez deamon
#/usr/sbin/telnetd -b 10.253.80.1 -p 3451 -l /usr/bin/ezdeamon.sh -F
/usr/sbin/telnetd -p 3451 -l /usr/bin/ezdeamon.sh -F

uci commit
sync

/sbin/wifi

robocfg switch enable

rm -f /tmp/.ezsetup-pluto
