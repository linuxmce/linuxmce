#!/bin/sh

mkdir -p /etc/pluto

DTVENDOR_FILE="/etc/pluto/Disked_DTVendor"
COMMAND_FILE="/etc/pluto/Disked_Command"
DEVDATA_FILE="/etc/pluto/Disked_DevData"

echo "rpi_b_orbiter" > $DTVENDOR_FILE
echo "select" > $COMMAND_FILE

TARGET_DISTRO=$(lsb_release -cs)
cat <<-EOF > /etc/lsb-release
        DISTRIB_ID=Raspbian
        DISTRIB_CODENAME=$TARGET_DISTRO
        EOF

