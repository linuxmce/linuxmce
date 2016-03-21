#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICEDATA_Configuration=59
DEVICEDATA_Alsa_Output_Device=74
DEVICEDATA_Pnp_Create_Without_Prompting=164
DEVICETEMPLATE_Squeezebox_Player=58
DEVICETEMPLATE_Squeezeslave=2148

# TODO: Disable/remove SqueezeBox Players for missing softsqueeze devices??

# Get list of squeezelites
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate = '$DEVICETEMPLATE_Squeezeslave'"
R=$(RunSQL "$Q")

for DEVICE in $R ; do
	# for each squeezelite
	#   if MAC not set
	#     create & set MAC
	Q="SELECT MACaddress FROM Device WHERE PK_Device='$DEVICE'"
	MAC=$(RunSQL "$Q")

	Q="SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device='$DEVICE'"
	PARENT=$(RunSQL "$Q")

	Q="SELECT MACaddress FROM Device WHERE PK_Device='$PARENT'"
	PARENT_MAC=$(RunSQL "$Q")

	if [[ -z "$PARENT_MAC" ]] ; then
		MyIF=$(/sbin/route -n | awk '/^0\.0\.0\.0/ { print $8 }')
		PARENT_MAC=$(/sbin/ifconfig "$MyIF" | awk 'NR==1 { print $5 }')
	fi

	# FIXME: Need to randomize some part of the MAC to handle multiple softsqueeze's on one pc
	# If there is no MAC set in the DB, we'll get it from the system
	if [[ -z "$MAC" ]] ; then
		MAC=$( awk -F: '{ print "00:00:00:"$4":"$5":"$6 }' <<<$PARENT_MAC )

		# Now write this to the database so user can see it in webadmin
		Q="UPDATE Device SET MACaddress='$MAC' WHERE PK_Device='$DEVICE'"
		R=$(RunSQL "$Q")
	fi
	[[ -z "$MAC" ]] && continue


	#   if Alsa_Output_Device not set
	#     set Alsa_Output_Device to default
	AUDIO_DEVICE=$(GetDeviceData "$DEVICE" "$DEVICEDATA_Alsa_Output_Device")
	if [[ -z "$AUDIO_DEVICE" ]] ; then
		SetDeviceData "$DEVICE" "$DEVICEDATA_Alsa_Output_Device" "default"
		AUDIO_DEVICE=$(GetDeviceData "$DEVICE" "$DEVICEDATA_Alsa_Output_Device")
	fi

	#echo "PK_Device: $DEVICE, MAC: $MAC, OUTPUT: $AUDIO_DEVICE"
	#echo "PK_Parent: $PARENT, MAC: $PARENT_MAC"

	#   if associated SqueezeBoxPlayer does not exist
	#     create player
	#     set MAC and room equal to squeezelite values

	# Check if we already are in the database, and if not, let the router create one
	# in our current room.
	Q="SELECT COUNT(*) FROM Device WHERE FK_DeviceTemplate='$DEVICETEMPLATE_Squeezebox_Player' AND MACaddress='$MAC';"
	INSTALLED=$(RunSQL "$Q")

	if [[ "$INSTALLED" == "0" ]]; then
		# We don't have the corresponding Squeezebox Player in our install
		# so let's create it.
		# We need the room the softsqueeze is in, so we don't bother the user with a question

		Q="SELECT FK_Room FROM Device WHERE PK_Device = $DEVICE"
		ROOM=$(RunSQL "$Q")

		echo "Installing SqueezeBox Player"
		echo /usr/pluto/bin/CreateDevice -h "$MySqlHost" -u "$MySqlUser" -D pluto_main -P "$MySqlPort" -d "$DEVICETEMPLATE_Squeezebox_Player" -i "$PK_Installation"
		NEW_DEVICE=$(/usr/pluto/bin/CreateDevice -h "$MySqlHost" -u "$MySqlUser" -D pluto_main -P "$MySqlPort" -d "$DEVICETEMPLATE_Squeezebox_Player" -i "$PK_Installation")

		if [[ -n "$NEW_DEVICE" ]] && [[ "$NEW_DEVICE" != "0" ]] ; then
			echo "New Device: $NEW_DEVICE"

			# Set MAC address
			echo "Setting MAC: $MAC"
			Q="UPDATE Device SET MACaddress='$MAC' WHERE PK_Device='$NEW_DEVICE'"
			RunSQL "$Q"

			# Set Room
			echo "Setting Room: $ROOM"
			Q="UPDATE Device SET FK_Room='$ROOM' WHERE PK_Device='$NEW_DEVICE'"
			RunSQL "$Q"

			echo "Installing SqueezeBox Player - SUCCESS!"
		else
			echo "Installing SqueezeBox Player - FAILED!"
			:
		fi
	fi
done

exit 0
