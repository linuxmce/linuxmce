#!/bin/dash

if [ -f /etc/Disked_DeviceID ]; then
	echo "We have a DeviceID already, bypassing interactor."
  exit 0
fi

MyIP=$(/sbin/ifconfig eth0 | awk 'NR==2 { print substr($2, index($2, ":") + 1) }')
MyMAC=$(/sbin/ifconfig eth0 | awk 'NR==1 { print $5 }')
Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')

ARCH=$(arch)
case $ARCH in
	i386|i686)
		Architecture="i386"
		;;
	amd64)
		Architecture="i386"
		;;
	armhf)
		Architecture="armhf"
		;;
	*)
		echo "ERR: unknown arch $ARCH"
		return 1
esac

devicedata=""
MyDeviceData="$Architecture""$devicedata"

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway; DeviceData: $MyDeviceData"

/sbin/interactor "$Gateway" "$MyIP" "$MyMAC" "$MyDeviceData"
