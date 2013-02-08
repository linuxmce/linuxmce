#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh

if [[ $1 != "background" ]] ;then
        echo "Backgrounding ..."
        screen -d -m -S "Internal IP Change" "$0" "background"
        exit 0
fi

. /usr/pluto/bin/TeeMyOutput.sh --outfile /var/log/pluto/InternalIPChange.log --infile /dev/null --stdboth --append -- "$@"


DD_DHCP=28

# Increments an IP addrees by 1
function IncIP() {
	local Address="$1"
	Address_1=$(echo "$Address" | cut -d'.' -f1)
	Address_2=$(echo "$Address" | cut -d'.' -f2)
	Address_3=$(echo "$Address" | cut -d'.' -f3)
	Address_4=$(echo "$Address" | cut -d'.' -f4)

	if [[ $Address_4 -lt 255 ]] ;then
		Address_4=$(( Address_4 + 1 ))
	elif [[ $Address_3 -lt 255 ]] ;then
		Address_3=$(( Address_3 + 1 ))
		Address_4=0
	elif [[ $Address_2 -lt 255 ]] ;then
		Address_2=$(( Address_2 + 1 ))
		Address_3=0
		Address_4=0
	elif [[ $Address_1 -lt 255 ]] ;then
		Address_1=$(( Address_1 + 1 ))
		Address_2=0
		Address_3=0
		Address_4=0
	fi
	
	echo "$Address_1.$Address_2.$Address_3.$Address_4"
}

# Read the starting ip address for the devices
R=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData where FK_Device=1 AND FK_DeviceData=$DD_DHCP")
DHCP_Range=$(Field 1 "$R")
DHCP_Range=$(echo $DHCP_Range | cut -d',' -f1)
DHCP_Start=$(echo $DHCP_Range | cut -d'-' -f1)

# Update the devices that have an ip address assigned excepting the core
R=$(RunSQL "SELECT PK_Device,IPaddress FROM Device WHERE IPaddress <> '' AND PK_Device <> 1");
Next_IP=$DHCP_Start
## Right now we'll assume that the netmask is always 255.255.255.0 when deciding the prefix
Prefix="${DHCP_Start%.*}" # just remove the fourth byte
for Device in $R ;do
	Device_ID=$(Field 1 "$Device");
	Device_IP=$(Field 2 "$Device")
	New_IP="$Prefix.${Device_IP##*.}"
	RunSQL "UPDATE Device SET IPaddress='$New_IP' WHERE PK_Device='$Device_ID' LIMIT 1"
	RunSQL "UPDATE Device SET NeedConfigure = 1 WHERE PK_Device='$Device_ID' LIMIT 1"
	RunSQL "UPDATE Device SET Status = '**RUN_CONFIG**' WHERE PK_Device='$Device_ID' LIMIT 1"
	
	Next_IP=$(IncIP $Next_IP)
done

# Mark all Phones as needing configure
RunSQL "UPDATE Device SET NeedConfigure = 1 WHERE FK_DeviceTemplate IN ($(ListTemplates_Category 89 | sed 's/ /,/g'))"
RunSQL "UPDATE Device SET Status = '**RUN_CONFIG**' WHERE FK_DeviceTemplate IN ($(ListTemplates_Category 89 | sed 's/ /,/g'))"
/usr/pluto/bin/Config_Device_Changes.sh

# Mark DisklessMDs as needing configure
RunSQL "UPDATE Device SET NeedConfigure=1 WHERE FK_DeviceTemplate=28"


# We should not run this
/usr/pluto/bin/StorageDevices_ExportsNFS.sh
/usr/pluto/bin/StorageDevices_ExportsSMB.sh
/usr/pluto/bin/Diskless_Setup.sh


## Generate a list containig all orbiter ids ($OrbiterIDList)
Q="
        SELECT
                PK_Device
        FROM
                Device
                JOIN DeviceTemplate ON PK_DeviceTemplate = FK_DeviceTemplate
                JOIN DeviceCategory ON PK_DeviceCategory = FK_DeviceCategory
        WHERE
                PK_DeviceCategory IN (5,2,3)
"
OrbiterList=$(RunSQL "$Q")

OrbiterIDList=""
for Orbiter in $OrbiterList; do
        if [[ $OrbiterIDList != "" ]]; then
                OrbiterIDList="$OrbiterIDList,"
        fi

        Orbiter_ID=$(Field 1 "$Orbiter")
        OrbiterIDList="$OrbiterIDList""$Orbiter_ID"
done

# Nofity the user that we're going to reboot the system
Message="You need to reboot the system in order to perform the IP changes that you requested."
/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 10 "UpdatesNotify" 159 53 183 0 9 "$Message" || exit 1

#sleep 10
#reboot
