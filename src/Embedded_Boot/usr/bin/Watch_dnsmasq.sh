#!/bin/ash

sleep 5

log_file="/tmp/log.dnsmasq"

echo "`date -R` - === Starting Watch Dnsmasq ===" >> $log_file

while :; do
	sleep 10
	dnsmasq_pid=`ps  | grep '/usr/sbin/dnsmasq' | grep -v grep | awk '{print $1}'`
	if [[ "$dnsmasq_pid" == "" ]]; then
		echo "`date -R` - Dnsmasq isn't running. Starting it..." >> $log_file
		/etc/init.d/dnsmasq start
	fi
done
