#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func

Device=$2

echo "$(date -R) ConfigureHDHomeRun start"

Q="SELECT IPaddress FROM Device WHERE PK_Device='$Device'"
IPAddress=$(RunSQL "$Q")

# Get the HDHomerun internal ID, which is normally used with the hdhomerun_config cli utility
DeviceID=$(/usr/pluto/bin/hdhomerun_config discover "$IPAddress" | grep "$IPAddress" | cut -d' ' -f3)

echo "$(date -R) HDHomeRun ID $DeviceID IP $IPAddress"

# Get a list of the tuners on the HDHomerun
Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Device'"
R=$(RunSQL "$Q")

for i in $R; do
	# Lets update the DeviceData for the tuners using the GeneralInfo plugin

	# First, we get the actual tuner number
	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$i' AND FK_DeviceData='12'"
	tuner=$(RunSQL "$Q")
	# And send our command
	/usr/pluto/bin/MessageSend dcerouter -targetType template -r 0 27 1 246 2 $i 52 152 5 "$DeviceID"-"$tuner"

done

# Force the new device id to be written into mythconverg setup
# with a SyncProvidersAndCards command to MythTV_Plugin
/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 824  

# Update the firmware
/usr/pluto/bin/hdhomerun_config $IPAddress upgrade /usr/pluto/bin/hdhomerun_atsc_firmware_20091024.bin

sleep 30  # give it time to recover after the firmware upgrade

echo "$(date -R) ConfigureHDHomeRun done"

#./hdhomerun_config 192.168.80.252 set /lineup/location us:94005  # opt-in to do channel scanning  /hdhomerun_config 192.168.80.252 set /lineup/location disabled
