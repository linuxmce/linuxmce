#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

PH="plutohome.com"

cronEntry="0 2 * * * root /usr/pluto/bin/Report_Machine_Status.sh"
#if ! grep -qF "$cronEntry" /etc/crontab; then
#	echo "$cronEntry" >>/etc/crontab
#	service cron reload
#fi

if [[ ! -e /etc/cron.d/ReportMachineStatus ]] ;then
	echo "$cronEntry" >>/etc/cron.d/ReportMachineStatus
	service cron reload
fi

Q="SELECT FK_DeviceTemplate
	FROM Device
	WHERE PK_Device='$PK_Device' LIMIT 1"
R=$(RunSQL "$Q")

PK_DeviceTemplate=$(Field 1 "$R")
Uptime=$(awk '{print $1}' /proc/uptime | cut -d. -f1)
Router=$(dpkg -s pluto-dcerouter 2>/dev/null | grep Version | cut -d" " -f2)
Orbiter=$(dpkg -s pluto-orbiter 2>/dev/null | grep Version | cut -d" " -f2)
Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$PK_Device AND FK_DeviceData=234"
R="$(RunSQL "$Q")"
UpdateVersion="$(Field 1 "$R")"

if [[ "$OfflineMode" == "true" ]]; then
	echo "$0: I am in offline mode, cannot wget, skiping."
else
	wget -t 1 --timeout=10 -O /dev/null "http://$PH/report_status.php?Installation=$PK_Installation&Device=$PK_Device&DeviceTemplate=$PK_DeviceTemplate&Uptime=$Uptime&Orbiter=$Orbiter&Router=$Router&UpdateVersion=$UpdateVersion" &>/dev/null
fi
