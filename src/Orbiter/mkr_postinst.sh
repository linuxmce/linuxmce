#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

DEVICECATEGORY_Standard_Orbiter=3
DEVICEDATA_PK_Distro=7

getorbiterid()
{
	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$PK_Device'"
	R=$(RunSQL "$Q")
	for ROW in ${R}; do
		device=$(Field 1 "$ROW")
		Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$device'"
		RES=$(RunSQL "$Q")
		R=${R}$'\n\t '${RES}
	done

	for ROW in ${R}; do
		device=$(Field 1 "$ROW")
		Q="SELECT PK_Device FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE FK_DeviceCategory=$DEVICECATEGORY_Standard_Orbiter AND (FK_Device_ControlledVia='$device' OR FK_Device_ControlledVia='$PK_Device')"
		Orbiter_Device=$(RunSQL "$Q")
		if [[ -n "$Orbiter_Device" ]]; then
			builtin echo $Orbiter_Device
			break
		fi
	done
}


Q="SELECT FK_DeviceCategory FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate WHERE PK_Device=$PK_Device"
DeviceCategory=$(RunSQL "$Q")

DeviceCategory_Core=7
DeviceCategory_MD=8

if [[ -n "$DeviceCategory" ]]; then
	if [[ $DeviceCategory -eq $DeviceCategory_MD ]]; then
		rm -f /etc/rc{0,6}.d/{S32portmap,S35networking}
	fi
fi

PrevVer="$2"

if [[ -n "$PrevVer" ]]; then
	echo "Upgrading from version '$PrevVer'. Not setting up X again"
fi

if [ -e /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc ] ;then
	flag='0'
	if ! grep -qF 'excludeApps=xfwm4' /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc ;then
			while read line ;do
					if [[ "$line" == "[General]" ]] ;then
							flag='1'
					fi
					if [[ "$flag" == "1" ]] ;then
							echo "$line" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$
							echo "excludeApps=xfwm4" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$
							flag='0'
					else
							echo "$line" >> /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$
					fi
			done < /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc
			mv /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc.$$ /usr/share/kubuntu-default-settings/kde-profile/default/share/config/ksmserverrc
	fi
fi

# Set DeviceData Distro properly for this orbiter
OrbiterID=$(getorbiterid)
if [[ -n "${OrbiterID}" ]]; then
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$PK_Distro' WHERE FK_Device='${OrbiterID}' AND FK_DeviceData='$DEVICEDATA_PK_Distro'"
	R=$(RunSQL "$Q")
fi

exit 0
