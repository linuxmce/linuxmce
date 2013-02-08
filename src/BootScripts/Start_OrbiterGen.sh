#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Utils.sh

trap 'Unlock "SOG" "Start_Orbiter_Gen"' EXIT
echo Orbiter Generation Start >>/dev/tty1
Lock "SOG" "Start_Orbiter_Gen"

UseAlternativeLibs

SkinDir=/usr/pluto/orbiter/skins
FontDir=/usr/share/fonts/truetype/msttcorefonts
OutDir=/usr/pluto/orbiter

# send progress messages to boot splash?
ToSplash="-b"

#<-mkr_b_ubuntu_b->
ToSplash=""
#<-mkr_b_ubuntu_e->

/usr/pluto/bin/UpdateEntArea $PLUTO_DB_CRED -D "$MySqlDBName" > >(tee -a /var/log/pluto/updateea.log)

Q="SELECT PK_Installation FROM Installation LIMIT 1"
installation=$(RunSQL "$Q;")

Q="SELECT FK_DeviceCategory FROM Device
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
WHERE PK_Device=$PK_Device"

if [ $(RunSQL "$Q;") == 7 ]; then

	# Generate for all the Orbiters with no parent (ie stand-alone orbiters)
	Q="SELECT PK_Device FROM Device 
JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
LEFT JOIN Orbiter ON PK_Device=PK_Orbiter
WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
AND (Regen IS Null Or Regen=1 Or Regen=2 OR PK_Orbiter IS NULL OR Device.NeedConfigure=1 OR RegenInProgress=1)
AND FK_Installation=$installation"

	Orbiters=$(RunSQL "$Q;")

	export SDL_VIDEODEVICE=dummy

	for OrbiterDev in $Orbiters; do
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Generating stand-alone Orbiter nr. $OrbiterDev"
		/usr/pluto/bin/OrbiterGen -d "$OrbiterDev" "$ToSplash" -g "$SkinDir" -f "$FontDir" -o "$OutDir" $PLUTO_DB_CRED -D "$MySqlDBName" > >(tee -a /var/log/pluto/orbitergen.log) 2> >(grep -vF "WARNING: You are using the SDL dummy video driver!" >&2) || Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Failed to generate Orbiter nr. $OrbiterDev"
	done
fi
echo Orbiter Generation Done >> /dev/tty1
