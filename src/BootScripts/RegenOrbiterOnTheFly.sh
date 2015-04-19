#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

MESSAGETYPE_Command=1
COMMAND_Regen_Orbiter=266
DEVICETEMPLATE_Orbiter_Plug_in=12

UseAlternativeLibs

regen_orbiter()
{
return 0
	local Dev
	Dev="$1"
echo "Regen_Orbiter $Dev $2 $3"

	Q="UPDATE Orbiter set RegenPercent=0 where PK_Orbiter=$Dev"
	RunSQL "$Q"
	/usr/pluto/bin/MessageSend "$DCERouter" 0 $Dev 7 1 163 "RegenOrbiterOnTheFly.sh"

	echo "on the fly regen of $Dev $2 $3" >> /var/log/pluto/orbitergen.log
	export SDL_VIDEODEVICE=dummy
	/usr/pluto/bin/OrbiterGen -d "$Dev" $3 -g "$SkinDir" -f "$FontDir" -o "$OutDir" $PLUTO_DB_CRED -D "$MySqlDBName"  >> /var/log/pluto/orbitergen.log || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $OrbiterDev"

	# Notify the Orbiter Plugin that we finished
	/usr/pluto/bin/MessageSend "$DCERouter" 0 $2 1 267 2 $Dev
}

SkinDir=/usr/pluto/orbiter/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto/orbiter

if [ "$1" = "0" ]; then
	Q="
	SELECT Device.PK_Device
	 FROM Device
	  JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
	  JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
	   WHERE
	    (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
	    AND Device.FK_Installation=$PK_Installation
	"
else
	Q="
	SELECT Device.PK_Device
	 FROM Device
	  WHERE PK_Device in ($1)
	"
fi
Orbiters=$(RunSQL "$Q")
for OrbiterDev in $Orbiters ; do
	ORB_ARRAY[$OrbiterDev]=0
done

# Do this on the core only
if [ "$PK_Device" = "1" ] ; then
	/usr/pluto/bin/UpdateEntArea $PLUTO_DB_CRED -D "$MySqlDBName" > >(tee -a /var/log/pluto/updateea.log)

	#get list of orbiters and their MDs
	Q="
	SELECT PK_Device, FK_Device_ControlledVia
	  FROM Device
	  JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
	  JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
	  WHERE PK_DeviceCategory=3
	    AND FK_Device_ControlledVia IS NOT NULL;
	"
	MDs=$(RunSQL "$Q")

	# Start 1 regen on each MD for now
	for MD in "$MDs" ; do
		ret=""

		ORB_DEV=$(Field "$MD" 1)
		MD_DEV=$(Field "$MD" 2)

		IPADDR=$(RunSQL "SELECT IPaddress FROM Device WHERE PK_Device=$MD_DEV")

		# ping to see if its alive
		ping -c1 -w1 -t100 "$IPADDR" >/dev/null 2>&1
		ret=$?
		if [ "$ret" = "0" ] ; then
			# MD is awake, tell it to regen
			# replace with messagesend to each md's app_server to run orbitergen
			echo "***REGEN ORBITER $ORB_DEV on MD $MD_DEV"
			#/usr/pluto/bin/MessageSend "$DCERouter" 0 $2 $MESSAGETYPE_Command $COMMAND_Regen_Orbiter 2 "$ORB_DEV"
			unset ORB_ARRAY[$ORB_DEV]
		fi
	done

	# Regen the rest on the core here
	# Start one on the core
	CORE_CPUS=$(nproc)
	remaining_orbiters="${#array[@]}"
	$((regen_split = remaining_orbiters / CORECPUS))

	for count in set 1..$CORE_CPUS ; do 
		ORBS=""
		for ORB_DEV in "${!ORB_ARRAY[@]}" ;do
			echo "$ORB_DEV ${ORB_ARRAY[index]}"
			if [ -n "$ORBS" ] ; then
				ORBS="$ORBS,"
			fi
			ORBS="$ORBS$ORB_DEV"
			unset "ORB_ARRAY[$ORB_DEV]"

			if [ $((i++)) >= regen_split ] ; break;
		done
		echo "***REGEN ORBITER: $ORBS on CORE"
		regen_orbiter $ORBS $2 $3
	done
else
	# Do this on all non-core devices
	for OrbiterDev in $Orbiters ; do
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating Orbiter on the fly nr. $OrbiterDev"
		regen_orbiter $OrbiterDev $2 $3
	done
fi

