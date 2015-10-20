#!/bin/dash
### BEGIN INIT INFO
# Provides:          interactor
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Should-Start:      
# Should-Stop:       
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Disked Interactor
# Description:       Interacts with LMCE Core
### END INIT INFO

DEVICEID_FILE="/etc/Disked_DeviceID"
DTVENDOR_FILE="/etc/pluto/Disked_DTVendor"
COMMAND_FILE="/etc/pluto/Disked_Command"
DEVDATA_FILE="/etc/pluto/Disked_DevData"

if [ -f "$DEVICEID_FILE" ]; then
	echo "Skipping interactor - we have a DeviceID already."
	exit 0
fi

MyIP=""
while [ -z "$MyIP" ] ; do
	MyIF=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $8 }')
	MyIP=$(/sbin/ifconfig $MyIF | awk 'NR==2 { print substr($2, index($2, ":") + 1) }')
	if [ -z "$MyIP" ] ; then
		echo "Waiting for network configuration to complete..."
		sleep 1
	fi
done
MyMAC=$(/sbin/ifconfig $MyIF | awk 'NR==1 { print $5 }')
Gateway=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $2 }')

# DeviceData Section
DEVICEDATA_Is_Diskless_Boot_CONST=9
DEVICEDATA_PK_Distro_CONST=7
DEVICEDATA_Operating_System_CONST=209
DEVICEDATA_Diskless_Archives_CONST=258
DEVICEDATA_Release_CONST=262

DEVICEDATA_DISTRO_Raspbian_Wheezy_CONST=19
DEVICEDATA_DISTRO_Raspbian_Jessie_CONST=22
DEVICEDATA_DISTRO_Ubuntu_Precise_CONST=20
DEVICEDATA_DISTRO_Ubuntu_Trusty_CONST=21

DEVICETEMPLATE_Generic_PC_as_MD_CONST=28

DD=$(arch)

# Disable diskless creation/boot
DD="$DD|$DEVICEDATA_Is_Diskless_Boot_CONST|0"

# get PK_Distro from OS
RELEASE=$(lsb_release -cs)
case "$RELEASE" in
	wheezy)
		release=$DEVICEDATA_DISTRO_Raspbian_Wheezy_CONST ;;
	jessie)
		release=$DEVICEDATA_DISTRO_Raspbian_Jessie_CONST ;;
	precise)
		release=$DEVICEDATA_DISTRO_Ubuntu_Precise_CONST ;;
	trusty)
		release=$DEVICEDATA_DISTRO_Ubuntu_Trusty_CONST ;;
	*)
		echo "ERR: Unknown release (distro)" ;;
esac
[ -n "$release" ] && DD="$DD|$DEVICEDATA_PK_Distro_CONST|$release"
[ -n "$RELEASE" ] && DD="$DD|$DEVICEDATA_Release_CONST|$RELEASE"

# get PK_Distro from OS
DISTRO=$(lsb_release -is)
[ -n "$DISTRO" ] && DD="$DD|$DEVICEDATA_Operating_System_CONST|$DISTRO"

# TODO: test this
#archives=""
#DD="$DD|$DEVICEDATA_Diskless_Archives_CONST|$archives"

DTVendorID=$DEVICETEMPLATE_Generic_PC_as_MD_CONST
Command="newmd"
if [ -f "$DTVENDOR_FILE" ]; then
	DTVendorID=$(cat "$DTVENDOR_FILE")
fi
if [ -f "$COMMAND_FILE" ]; then
	Command=$(cat "$COMMAND_FILE")
fi
if [ -f "$COMMAND_FILE" ]; then
	Extra_DD=$(cat "$DEVDATA_FILE")
	DD="$DD|$Extra_DD"
fi

echo "IP: $MyIP; MAC: $MyMAC; Gateway: $Gateway; DeviceData: $DD; DT/Vendor: $DTVendorID; Command: $Command"

/usr/pluto/bin/interactor-disked "$Gateway" "$MyIP" "$MyMAC" "$DD" "$DTVendorID" "$Command"
