#!/bin/sh

mkdir -p /etc/pluto

DTVENDOR_FILE="/etc/pluto/Disked_DTVendor"
COMMAND_FILE="/etc/pluto/Disked_Command"
DEVDATA_FILE="/etc/pluto/Disked_DevData"

# DT 2343 = Intel Compute Stick
echo "2343" > $DTVENDOR_FILE

cat <<- EOF >/etc/network/interfaces
	# interfaces(5) file used by ifup(8) and ifdown(8)
	auto lo
	iface lo inet loopback

	auto eth0
	iface eth0 inet dhcp

	#auto wlan0
	#iface wlan0 inet dhcp
	#wpa-ssid "SSID_NAME"
	#wpa-psk "PASSPHRASE"
	EOF
