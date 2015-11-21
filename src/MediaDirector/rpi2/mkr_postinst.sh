#!/bin/sh

mkdir -p /etc/pluto

DTVENDOR_FILE="/etc/pluto/Disked_DTVendor"
COMMAND_FILE="/etc/pluto/Disked_Command"

#StatsMessage "Creating softlink for libraspberypi"
ln -f -s /usr /opt/vc || :

echo "rpi_b" > $DTVENDOR_FILE
echo "select" > $COMMAND_FILE

update-rc.d -f switch_cpu_governor defaults >/dev/null

TARGET_DISTRO=$(lsb_release -cs)
cat <<-EOF > /etc/lsb-release
	DISTRIB_ID=Raspbian
	DISTRIB_CODENAME=$TARGET_DISTRO
	EOF

echo "gpu_mem=128" >> /boot/config.txt
