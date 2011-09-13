#!/bin/bash

PlutoHome="www.linuxmce.org"

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICEDATA_Remote_Assistance_Ports=212

if [[ -z "$PK_Installation" || "$PK_Installation" == 1 ]]; then
	echo "Unusable installation number: '$PK_Installation'"
	exit 1
fi

if [[ "$#" -ne 1 ]]; then
	echo "Usage: '$0' {--enable|--disable|--status|--confirm}"
	exit 1
fi

PortPage="http://$PlutoHome/get_ra_ports.php?installationID=$PK_Installation"

UpdatePortDeviceData()
{
	local IK_DeviceData="$1"
	local Time=$(date +%s)

	if [[ -n "$IK_DeviceData" ]]; then
		IK_DeviceData="$IK_DeviceData;time=$Time"
	fi

	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES ('$PK_Device', '$DEVICEDATA_Remote_Assistance_Ports', '$IK_DeviceData')
	"
	RunSQL "$Q"
}

RequestPorts()
{
	local IK_DeviceData Q password Var PortName PortValue line
	local PortFile="$(mktemp /tmp/tmp.RA-XXXXXX)"
	trap "rm -f '$PortFile'" EXIT

	if ! wget -O "$PortFile" "$PortPage" 2>/dev/null; then
		echo "Failed to get ports from server"
		UpdatePortDeviceData ""
		return 1
	fi

	if [[ "$(head -1 "$PortFile")" != "#Ports:" || "$(tail -1 "$PortFile")" != "#END" ]]; then
		echo "Received incomplete response"
		UpdatePortDeviceData ""
		return 1
	fi

	while read line; do
		if [[ "$line" == "#"* ]]; then
			continue
		fi
		eval "Port_$line"
	done <"$PortFile"

	IK_DeviceData=
	for Var in ${!Port_*}; do
		PortName="${Var#Port_}"
		PortValue="${!Var}"
		IK_DeviceData="${IK_DeviceData}${PortName}=${PortValue},"
	done
	IK_DeviceData="${IK_DeviceData%,}"
	UpdatePortDeviceData "$IK_DeviceData"
	return 0
}

EnableRA()
{
	if ! RequestPorts; then
		ConfDel remote
		echo "Failed to enable Remote Assistance"
		return 1
	fi

	if grep -q '^remote.*=' /etc/pluto.conf; then
		password=$(grep '^remote.*=' /etc/pluto.conf)
		password=${password#*=}
	else
		. /usr/pluto/bin/Utils.sh
		password="$(GeneratePasswordOf6Digits)"
		ConfSet remote "$password"
	fi

	/usr/pluto/bin/SetupRemoteAccess.sh
	echo "RA enabled"
}

DisableRA()
{
	local Q

	ConfDel remote
	/usr/pluto/bin/SetupRemoteAccess.sh
	UpdatePortDeviceData ""
	wget -O /dev/null "$PortPage&action=del" 2>/dev/null
}

ShowStatus()
{
	if [[ -n "$remote" ]]; then
		echo "Remote Assistance is enabled"
		return 0
	else
		echo "Remote Assistance is disabled"
		return 1
	fi
}

RetCode=0
for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--enable)
			EnableRA
		;;
		--disable)
			DisableRA
		;;
		--status)
			ShowStatus
			RetCode=$?
		;;
		--confirm)
			if [[ -z "$remote" ]]; then
				# nothing to do
				continue
			fi
			RequestPorts
			RetCode=$?
		;;
		*)
			echo "Unknown parameter: '${!i}'"
			RetCode=1
		;;
	esac
done
exit $RetCode
