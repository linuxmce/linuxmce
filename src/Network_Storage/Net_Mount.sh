#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DEVICEDATA_Type=47
DEVICEDATA_Share_Name=126
DEVICEDATA_Username=127
DEVICEDATA_Password=128
DEVICEDATA_Mount_Point=129
DEVICEDATA_Use_Pluto_Directory_Structure=130

while [[ "$#" > 0 ]]; do
	case "$1" in
		-d) NAS_PK_Device="$2"; shift ;;
		-i) NAS_IP_Address="${Params[$i]}"; shift ;;
		-m) NAS_MAC_Address="${Params[$i]}"; shift ;;
		*) Logging "NAS" $SEVERITY_WARNING "share mount" "Unknown parameter '$1'" ;;
	esac
	shift
done

#Create a way to determine if MythTV is installed...
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=36"
MythTV_Installed=$(RunSQL "$Q")

if [[ -z "$NAS_PK_Device" ]]; then
	Logging "NAS" $SEVERITY_CRITICAL "share mount" "I didn't receive my device number as a parameter. Ending"
	exit 1
fi

if [[ -z "$NAS_IP_Address" ]]; then
	SQL="SELECT IPaddress FROM Device WHERE PK_Device=$NAS_PK_Device"
	NAS_IP_Address=$(RunSQL "$SQL")
fi

SQL="SELECT FK_DeviceData,IK_DeviceData FROM Device_DeviceData
	WHERE FK_Device=$NAS_PK_Device AND FK_DeviceData IN ($DEVICEDATA_Type,$DEVICEDATA_Share_Name,$DEVICEDATA_Username,$DEVICEDATA_Password,$DEVICEDATA_Mount_Point,$DEVICEDATA_Use_Pluto_Directory_Structure)"
R=$(RunSQL "$SQL")

for Row in $R; do
	FK_DeviceData=$(Field 1 "$Row")
	IK_DeviceData=$(Field 2 "$Row")

	case "$FK_DeviceData" in
		$DEVICEDATA_Type) NAS_Type="$IK_DeviceData" ;;
		$DEVICEDATA_Share_Name) NAS_Share_Name="$IK_DeviceData" ;;
		$DEVICEDATA_Username) NAS_Username="$IK_DeviceData" ;;
		$DEVICEDATA_Password) NAS_Password="$IK_DeviceData" ;;
		$DEVICEDATA_Mount_Point) NAS_Mount_Point="$IK_DeviceData" ;;
		$DEVICEDATA_Use_Pluto_Directory_Structure) NAS_Use_Pluto_Directory_Structure="$IK_DeviceData" ;;
		*) Logging "NAS" $SEVERITY_WARNING "share mount" "Unknown DeviceData '$FK_DeviceData=$IK_DeviceData'" ;;
	esac
done

SQL="SELECT Description FROM Device WHERE PK_Device=$NAS_PK_Device"
R=$(RunSQL "$SQL")
NAS_Description=$(Field 1 "$R")

Msg="Description: $NAS_Description; Type: $NAS_Type; IP: $NAS_IP_Address; Share name: $NAS_Share_Name; Username: $Username; Mount point: $NAS_Mount_Point; Pluto directory structure: $NAS_Use_Pluto_Directory_Structure"
Logging "NAS" $SEVERITY_NORMAL "share mount" "$Msg"

if [[ -z "$NAS_IP_Address" || -z "$NAS_Share_Name" ]]; then
	Logging "NAS" $SEVERITY_CRITICAL "share mount" "Missing important mount parameters"
	exit 1
fi

Dst="/mnt/${NAS_Description}_${NAS_PK_Device}"
mkdir -p "$Dst"

case "$NAS_Type" in
	nfs)
		FS="nfs"
		Src="$NAS_IP_Address:$NAS_Share_Name"
		Opts="intr,rsize=32768,wsize=32768,retrans=10,timeo=50" # tcp doesn't seem to allow mounting most of the time with the Maxtor Network Storage
	;;
	samba|smbfs)
		FS="smbfs"
		Opts="password=$NAS_Password"
		Src="//$NAS_IP_Address/$NAS_Share_Name"
		[[ -n "$NAS_Username" ]] && Opts="$Opts,username=$NAS_Username"
	;;
	*)
		Logging "NAS" $SEVERITY_CRITICAL "share mount" "Unknown share type: '$NAS_Type'"
		exit 1
	;;
esac

if ! mount -t "$FS" -o "$Opts" "$Src" "$Dst"; then
	Logging "NAS" $SEVERITY_CRITICAL "share mount" "Mount failed"
else
	Logging "NAS" $SEVERITY_WARNING "share mount" "Mount succeeded"

	LinkName="NAS_${NAS_Description}_$NAS_PK_Device"
	
	#find /home -type l -name "$LinkName" -exec rm -f '{}' ';' # clean up links
	
	if [[ -n "$NAS_Use_Pluto_Directory_Structure" && "$NAS_Use_Pluto_Directory_Structure" -gt 0 ]]; then
		Logging "NAS" $SEVERITY_NORMAL "share mount" "Using Pluto directory structure"
		/usr/pluto/bin/SetupUsers_Homes.sh -n -b "$Dst" -e /home "$LinkName"
	elif [[ -n "$NAS_Mount_Point" ]]; then
		Logging "NAS" $SEVERITY_NORMAL "share mount" "Not using Pluto directory structure. Specific mount point(s): '$NAS_Mount_Point'"
		for entries in ${NAS_Mount_Point//,/ }; do
			user="${entries%%/*}"
			dir="${entries#*/}"
			[[ "$user" != public ]] && user="user_$user"
			Target="/home/$user/data/$dir/$LinkName"
			ln -sf "$Dst" "$Target"
		done
	else # not using Pluto directory structure, no mount point specified
		Logging "NAS" $SEVERITY_NORMAL "share mount" "Not using Pluto directory structure. Links all over the place"
		
		Q="SELECT PK_Users FROM Users"
		R=$(RunSQL "$Q")

		for user in public $R; do
			[[ "$user" == public ]] || user="user_$user"
			for dir in $LMCE_DIRS; do
				Target="/home/$user/data/$dir/$LinkName"
				ln -sf "$Dst" "$Target"
			done
		done
	fi
fi
