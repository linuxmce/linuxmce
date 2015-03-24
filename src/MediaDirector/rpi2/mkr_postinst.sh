#!/bin/sh

mkdir -p /etc/pluto

DTVENDOR_FILE="/etc/pluto/Disked_DTVendor"
COMMAND_FILE="/etc/pluto/Disked_Command"

echo "rpi_b" > $DTVENDOR_FILE
echo "select" > $COMMAND_FILE

update-rc.d -f switch_cpu_governor start 50 2 3 4 5 . >/dev/null



