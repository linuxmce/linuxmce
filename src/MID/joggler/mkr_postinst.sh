#!/bin/sh

mkdir -p /etc/pluto

DTVENDOR_FILE="/etc/pluto/Disked_DTVendor"
COMMAND_FILE="/etc/pluto/Disked_Command"
DEVDATA_FILE="/etc/pluto/Disked_DevData"

echo "2176" > $DTVENDOR_FILE
echo "newmd" > $COMMAND_FILE
echo "100|800|101|480" > $DEVDATA_FILE

# ensure the mac address stays consistent, without this it randomizes at boot
cat <<-EEOF > /etc/network/if-pre-up.d/openframe-mac
	#!/bin/bash

	# Ensure udev doesn't join the party.
	[ -f /etc/udev/rules.d/70-persistent-net.rules ] && rm /etc/udev/rules.d/70-persistent-net.rules
	[ -f /etc/udev/rules.d/75-persistent-net-generator.rules ] || touch /etc/udev/rules.d/75-persistent-net-generator.rules

	if [ "$IFACE" = "eth0" ]; then
		if [ -f /etc/network/mac_eth0 ]; then
			ETHMAC=`cat /etc/network/mac_eth0`
		else
			ETHMAC="02:`cat /proc/interrupts | md5sum | sed -r "s/(.{2})/\1:/g; s/^(.{14}).*/\1/"`"
			echo $ETHMAC > /etc/network/mac_eth0
		fi

		ip link set eth0 address $ETHMAC
		echo $ETHMAC > /tmp/mac_eth0
	fi

	if [ "$IFACE" = "wlan0" ]; then
		if [ -f /etc/network/mac_wlan0 ]; then
			ETHMAC=`cat /etc/network/mac_wlan0`
		else
			ETHMAC="00:`cat /proc/interrupts | md5sum | sed -r "s/(.{2})/\1:/g; s/^(.{14}).*/\1/"`"
			echo $ETHMAC > /etc/network/mac_wlan0
		fi

		ip link set wlan0 address $ETHMAC
		echo $ETHMAC > /tmp/mac_wlan0
	fi

	exit 0
	EEOF

