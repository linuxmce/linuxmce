#!/bin/bash

. /usr/pluto/bin/pluto.func

#$0 <MAC> <filename>

if [ "$#" -ne "2" ]; then
	echo "Usage: $0 <MAC> <filename>"
	exit
fi

MAC="$1"
FILE="$2"

#TODO: check if MAC is valid

if [ ! -f "$FILE" -o ! -r "$FILE" ]; then
	echo "file '$FILE' not found or not accessible"
	exit
fi

#/usr/pluto/bin/ussp-push /dev/rfcomm0 "$SIS" PlutoMO.sis
#(echo "c"; echo "p"; echo "$SIS PlutoMO.sis"; echo "q") | obex_test -b $MAC 9 |grep -qF 'PUT successful!'
# Some phones need us to use the OBEX Push channel to accept files without first
# being paired with us
CHANNEL=`sdptool search --bdaddr "$MAC" OPUSH | grep Channel | cut -f6 -d" "`
obexftp --nopath --noconn --uuid none -b "$MAC" --channel $CHANNEL -p "$FILE" 2>&1 | grep "Sending \"$FILE\"..." | grep -qF 'done'
Ret=$?

if [[ "$Ret" -eq 0 ]]; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "File '$FILE' uploaded to '$MAC'"
	exit 0
else
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "File '$FILE' failed to upload to '$MAC'"
	exit 1
fi
