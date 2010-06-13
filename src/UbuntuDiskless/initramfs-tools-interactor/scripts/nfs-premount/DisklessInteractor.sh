#!/bin/sh

exec 1>&2

DT=$(/scripts/DeviceTemplate.sh)

MyIP=`ifconfig eth0 | grep "inet addr" | sed -r 's/^.* inet addr:([0-9.]*).*/\1/g'`
MyMAC=`ifconfig eth0 | grep "HWaddr "  | sed -r 's/^.* HWaddr ([a-zA-Z0-9:]*)/\1/g'`
Gateway=`ip route | grep "default via" | cut -d " " -f3`

Architecture="i386"

#Temporarily Removed until amd64 builds are re-instated. -tschak
#if grep -q "^flags.* lm" /proc/cpuinfo ;then
#	Architecture="amd64"
#fi

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway; Architecture: $Architecture; DT: $DT"
/scripts/interactor "$Gateway" "$MyIP" "$MyMAC" "$Architecture" "$DT"

clear
echo "ERROR: Cannot connect to router, rebooting in 5 seconds.."
sleep 5
reboot
