#!/bin/sh

mkdir -p /etc/pluto

DTVENDOR_FILE="/etc/pluto/Disked_DTVendor"
COMMAND_FILE="/etc/pluto/Disked_Command"
DEVDATA_FILE="/etc/pluto/Disked_DevData"

echo "2176" > $DTVENDOR_FILE
echo "newmd" > $COMMAND_FILE
echo "100|800|101|480" > $DEVDATA_FILE



