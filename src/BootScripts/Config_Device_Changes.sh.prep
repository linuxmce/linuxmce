#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Utils.sh

#<-mkr_b_via_b->
ConfSet PK_Distro 1
#<-mkr_b_via_e->

LogFile="/var/log/pluto/Config_Device_Changes.log"
. /usr/pluto/bin/TeeMyOutput.sh --outfile "$LogFile" --stdboth --append --exclude "^W: GPG error" -- "$@"

echo "-----------------------------------"
date -R

NeedConfigure()
{
	local Device NeedConfigure PK_Device
	local i

	Device="$1"

	Q="SELECT NeedConfigure FROM Device WHERE PK_Device='$Device'"
	R=$(RunSQL "$Q")
	NeedConfigure=$(Field 1 "$R")

	if [[ "$NeedConfigure" == 1 ]]; then
		return 0
	fi

	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Device'"
	R=$(RunSQL "$Q")
	for i in $R; do
		if NeedConfigure $i; then
			return 0
		fi
	done
	
	return 1
}

Unset_NeedConfigure_Children()
{
	local Device
	local i

	Device="$1"
#	Q="UPDATE Device SET NeedConfigure=0 WHERE PK_Device='$Device'" # this line resets the orbiters and OrbiterGen doesn't run anymore
	Q="UPDATE Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory ON FK_DeviceCategory=DeviceCategory.PK_DeviceCategory
		LEFT JOIN DeviceCategory AS DeviceCategory2 ON DeviceCategory2.PK_DeviceCategory=DeviceCategory.FK_DeviceCategory_Parent
		LEFT JOIN DeviceCategory AS DeviceCategory3 ON DeviceCategory3.PK_DeviceCategory=DeviceCategory2.FK_DeviceCategory_Parent
		SET NeedConfigure=0,Device.psc_mod=Device.psc_mod
		WHERE (DeviceCategory.PK_DeviceCategory<>5 OR DeviceCategory.PK_DeviceCategory IS NULL)
		AND (DeviceCategory2.PK_DeviceCategory<>5 OR DeviceCategory2.PK_DeviceCategory IS NULL)
		AND (DeviceCategory3.PK_DeviceCategory<>5 OR DeviceCategory3.PK_DeviceCategory IS NULL)
		AND (DeviceCategory3.FK_DeviceCategory_Parent<>5 OR DeviceCategory3.FK_DeviceCategory_Parent IS NULL)
		AND PK_Device='$Device'"
	RunSQL "$Q"

	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Device'"
	R=$(RunSQL "$Q")
	for i in $R; do
		Unset_NeedConfigure_Children $i
	done
}

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		F) Force=y ;;
		StartLocalDevice) StartLocalDevice=y ;;
		Alert) Alert=y ;;
		*) echo "Unrecognized parameter '${!i}' skipped." ;;
	esac
done

trap 'Unlock "CDC" "Config_Device_Changes"' EXIT
WaitLock "CDC" "Config_Device_Changes" # don't run two copies of CDC simultaneously

if [[ "$Force" != "y" ]]; then
	echo "$(date -R) --> NeedConfigure $Device"
	NeedConfigure "$PK_Device"
	Ret=$?
	echo "$(date -R) <-- NeedConfigure $Device"
	if [[ "$Ret" -ne 0 ]]; then
		exit 0
	fi
fi

Orbiter_Alert=""
if [[ "$Alert" == "y" ]]; then
	Q="SELECT Device.PK_Device FROM Device LEFT JOIN Device as P1 ON Device.FK_Device_ControlledVia=P1.PK_Device WHERE Device.FK_DeviceTemplate=62 AND (Device.FK_Device_ControlledVia='$PK_Device' OR P1.FK_Device_ControlledVia='$PK_Device')"
	R=$(RunSQL "$Q")
	Orbiter=$(Field 1 "$R")
	if [[ "$Orbiter" != "" ]]; then
		Orbiter_Alert="-O $Orbiter"
	fi
fi

[ -n "$MySqlPassword" ] && Pass="-p$MySqlPassword"
CUsh="/usr/pluto/install/config_update.sh"

if [[ "$PK_Distro" != "" ]] ;then
	ConfDep_Distro="-o $PK_Distro"
else
	ConfDep_Distro=
fi

echo /usr/pluto/bin/ConfirmDependencies $ConfDep_Distro -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device $Orbiter_Alert install
/usr/pluto/bin/ConfirmDependencies $ConfDep_Distro -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device $Orbiter_Alert install >"$CUsh.$$"
linecount=$(cat "$CUsh.$$" | wc -l)
awk "NR<$linecount-4" "$CUsh.$$" >"$CUsh"
rm "$CUsh.$$"

chmod +x "$CUsh"
WaitLock "InstallNewDevice" "Config_Device_Changes" # don't step on InstallNewDevices scripts that may be running in the background
date -R
if bash -x "$CUsh"; then
	Unset_NeedConfigure_Children "$PK_Device"
fi
Unlock "InstallNewDevice" "Config_Device_Changes"
#rm "$CUsh"

echo /usr/pluto/bin/ConfirmDependencies $ConfDep_Distro -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device $Orbiter_Alert buildall
mkdir -p /usr/pluto/sources

: >"/usr/pluto/sources/buildall.sh"
echo '#!/bin/bash' >>"/usr/pluto/sources/buildall.sh"
echo "cd /usr/pluto/sources" >>"/usr/pluto/sources/buildall.sh"
/usr/pluto/bin/ConfirmDependencies $ConfDep_Distro -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device $Orbiter_Alert buildall >>"/usr/pluto/sources/buildall.sh"
rm -f "/usr/pluto/install/compile.sh" # old version mistake precaution
ln -sf "/usr/pluto/sources/buildall.sh" "/usr/pluto/install/compile.sh"
chmod +x "/usr/pluto/sources/buildall.sh"

if [[ "$StartLocalDevice" == "y" ]]; then
        echo "$(date -R) Starting local devices $DCERouter $PK_Device"
        /usr/pluto/bin/MessageSend "$DCERouter" 0 "$PK_Device" 7 12
fi

#<-mkr_b_via_b->
# Run alsaconf-noninteractive
if [[ -x /usr/pluto/bin/alsaconf-noninteractive ]]; then
	/usr/pluto/bin/alsaconf-noninteractive
	amixer sset 'Master Front' 74% unmute
fi
#<-mkr_b_via_e->
