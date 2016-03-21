#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DEVICEDATA_PK_Distro_CONST=7

GetCurrentDistro() {
	local DISTRO=$(lsb_release -cs)
	case $DISTRO in
		precise) CurDistro="20" ;;
		trusty) CurDistro="21" ;;
		xenial) CurDistro="23" ;;
		wheezy) CurDistro="19" ;;
		jessie) CurDistro="22" ;;
		stretch) : ;;
		buster) : ;;
	esac

	echo $CurDistro
}

VerifyDeviceDistroDB() {
	local Device="$1"
	local CurDistro="$2"

	echo "Checking device $Device"
	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device = '$dev' AND FK_DeviceData = '$DEVICEDATA_PK_Distro_CONST'"
	R=$(RunSQL "$Q")
	if [ -n "$R" ] && [ "$R" != "$CurDistro" ] ; then
		echo "Updating device PK_Device = $Device, old distro = $R, CurDistro = $CurDistro"
		Q="UPDATE Device_DeviceData SET IK_DeviceData = '$CurDistro' WHERE FK_Device = '$dev' AND FK_DeviceData = '$DEVICEDATA_PK_Distro_CONST'"
		RunSQL "$Q"
	fi

	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia = '$Device'"
	Children=$(RunSQL "$Q")

[[ -n "$Children" ]] && echo "Check device $Device's children"
	for dev in $Children ; do
		VerifyDeviceDistroDB $dev $CurDistro
	done
}  

VerifyDeviceDistro() {
	#check current distro
	local CurDistro=$(GetCurrentDistro)
	echo "PK_Device = $PK_Device, CurDistro = $CurDistro, PK_Distro = $PK_Distro"
	if [ "$PK_Distro" != "$CurDistro" ] ; then
		echo "Updating pluto.conf, PK_Distro = $PK_Distro, CurDistro = $CurDistro"
		ConfSet "PK_Distro" "$CurDistro"
	fi

	VerifyDeviceDistroDB $PK_Device $CurDistro
}

VerifyDeviceDistro

exit 0
