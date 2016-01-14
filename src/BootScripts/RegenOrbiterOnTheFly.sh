#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

UseAlternativeLibs

DEVICETEMPLATE_Orbiter_Plug_in_CONST=12
DEVICEDATA_Number_of_jobs_CONST=324

/usr/pluto/bin/UpdateEntArea $PLUTO_DB_CRED -D "$MySqlDBName" > >(tee -a /var/log/pluto/updateea.log)

Q="SELECT IK_DeviceData FROM Device_DeviceData
  WHERE FK_Device IN
    (SELECT PK_Device FROM Device
      WHERE FK_DeviceTemplate=$DEVICETEMPLATE_Orbiter_Plug_in_CONST
      AND FK_Installation=$PK_Installation)
  AND FK_DeviceData=$DEVICEDATA_Number_of_jobs_CONST"
Jobs=$(RunSQL "$Q")
[[ -z "$Jobs" ]] && Jobs=1

if [ "$1" = "0" ]; then
	Q="SELECT Device.PK_Device FROM Device
	JOIN DeviceTemplate ON Device.FK_DeviceTemplate=PK_DeviceTemplate
	JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
	WHERE (FK_DeviceCategory=5 OR FK_DeviceCategory_Parent=5)
#	AND Device.FK_DeviceTemplate NOT IN
#		( SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE Description LIKE '%qorbiter%' )
	AND Device.FK_Installation=$PK_Installation"
else
	Q="SELECT Device.PK_Device FROM Device
	WHERE PK_Device in ($1)
#	AND Device.FK_DeviceTemplate NOT IN 
#		( SELECT PK_DeviceTemplate FROM DeviceTemplate WHERE Description LIKE '%qorbiter%' )
	"
fi
Orbiters=$(RunSQL "$Q")

if [[ -n "$Orbiters" ]] ; then
	parallel -j "$Jobs" /usr/pluto/bin/RegenOrbiter.sh {1} "$2" ::: $Orbiters
fi

exit 0
