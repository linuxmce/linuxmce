#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh

keepAliveCmd="/usr/pluto/bin/RA_KeepPortsAlive.sh"
cronCmd="/usr/pluto/bin/SetupRemoteAccess.sh"
cronCmd_Special="/usr/pluto/bin/SetupRA-Special.sh"
cronEntry="*/1 * * * * root bash -c '$cronCmd &>/dev/null'"
cronEntry_Special="*/10 * * * * root bash -c '$cronCmd_Special &>/dev/null'"
screenName="RemoteAssistance"

RAServer=ra.linuxmce.org
RAKey="/usr/pluto/keys/id_dsa_remoteassistance"

DEVICEDATA_Remote_Assistance_Ports=212

UpdateInterval=86400 #seconds

GetPorts()
{
	Q="
		SELECT IK_DeviceData
		FROM Device_DeviceData
		WHERE
			FK_Device='$PK_Device'
			AND FK_DeviceData='$DEVICEDATA_Remote_Assistance_Ports'
	"
	Result="$(RunSQL "$Q")"
	if [[ "$Result" != *';'* ]]; then
		Result="$Result;time=0"
	fi
	Ports="${Result%%;*}"
	Vars="${Result#*;}"
}

GetPorts
for Var in ${Vars//,/ }; do
	if [[ "$Var" == "time="* ]]; then
		LastUpdate="${Var#time=}"
		Now="$(date +%s)"
		if ((Now - LastUpdate < 0 || Now - LastUpdate > UpdateInterval)); then
			/usr/pluto/bin/RA-handler.sh --confirm
			GetPorts
		fi
		break
	fi
done

for Port in ${Ports//,/ }; do
	if [[ "$Port" == "nomon_"* ]]; then
		eval "PortNoMon_${Port#nomon_}"
	else
		eval "Port_$Port"
	fi
done

if [[ ! -L "$cronCmd_Special" ]]; then
	rm -f "$cronCmd_Special"
	ln -s "$cronCmd" "$cronCmd_Special"
fi

[[ -f "$RAKey" ]] && chmod 700 "$RAKey" || exit

shopt -s nullglob



AddCronEntry()
{
	local CronReload

	if [[ ! -e /etc/cron.d/SetupRemoteAccess ]] ;then
		echo "$cronEntry" >>/etc/cron.d/SetupRemoteAccess
		CronReload="1"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Added crontab entry"				
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry already present. Not adding."
	fi
	
	#if [[ ! -e /etc/cron.d/SetupRA-Special ]] ;then
	#	echo "$cronEntry_Special" >>/etc/cron.d/SetupRA-Special
	#	CronReload="1"
	#    Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Added crontab entry (special)"
	#else
	#	Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) already present. Not adding."
	#fi

	if [[ -n "$CronReload" ]]; then
		service cron reload
	fi
	
}

DelCronEntry()
{
	local CronReload

	if [[ -e /etc/cron.d/SetupRemoteAccess ]] ;then
		rm -f /etc/cron.d/SetupRemoteAccess
		CronReload="1"
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry found. Removed."		 
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry not found. Not removing."
	fi
	
	if [[ -e /etc/cron.d/SetupRA-Special ]] ;then
	    rm -f /etc/cron.d/SetupRA-Special
	    CronReload="1"
	    Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) found. Removed."
	else
	    Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Crontab entry (special) not found. Not removing."
	fi
													

	if [[ -n "$CronReload" ]]; then
		service cron reload
	fi
}

CreateTunnel()
{
	Suffix="$1"
	LocalPort="$2"
	RemotePort="$3"
	Host="$4"
	RAhost="${5:-ra.linuxmce.org}"
	RAport="$6"
	Monitored="${7:-yes}"

	Tunnel=$(echo /var/run/screen/S-root/*.${screenName}_${Suffix})

	# if tunnel is not active create it
	Dead=0
	if [[ "$Monitored" == yes && "$RA_CheckRemotePort" == "1" ]]; then
		# Foxi352 - 29 sep 2011 - disabled nc because on ra host we only bind to localhost
		#if [[ -z "$Tunnel" ]] || ! nc -z -w1 "$RAhost" "$RemotePort"; then
		if [[ -z "$Tunnel" ]]; then
			Dead=1
		fi
	else
		if [[ -z "$Tunnel" ]]; then
			Dead=1
		fi
	fi
	if [[ "$Dead" -eq 1 ]]; then
		FalseAlarm=0
		if [[ -n "$Tunnel" && "$RA_CheckRemotePort" == "1" && "$Monitored" == yes ]]; then
			for ((i=0;i<5;i++)); do
				nc -z -w1 "$RAhost" "$RemotePort" && FalseAlarm=1 && break
				sleep 1
			done
		fi
		if [[ "$FalseAlarm" -eq 0 ]]; then
			[[ -n "$Tunnel" ]] && RemoveTunnel "$Suffix"
			screen -d -m -S "${screenName}_${Suffix}" /usr/pluto/bin/RemoteAccess_Tunnel.sh "$RemotePort" "$LocalPort" "$RAKey" "$Host" "$RAhost" "$RAport"
			Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel enabled."
		else
			Logging "$TYPE" "$SEVERITY_WARNING" "$0" "$Suffix tunnel not down. False alarm"
		fi
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel already present. Not enabling."
	fi
}

RemoveTunnel()
{
	Suffix="$1"
	Tunnel=$(echo /var/run/screen/S-root/*${screenName}_${Suffix})

	# kill port forward if it exists
	if [[ -n "$Tunnel" ]]; then
		PID="${Tunnel##*/}"
		PID="${PID%%.*}"
		kill $PID
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel found. Closed."
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "$Suffix tunnel not found. Not closing."
	fi
}

CreateTunnels()
{
	/usr/pluto/bin/RA_ChangePassword.sh

	local Var PortName PortTunnel PortNameDest PortDest

	for Var in ${!Port_*}; do
		PortNameDest="${Var#Port_}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${!Var}"
		#echo "CreateTunnel \"${PortName}_pf\" \"$PortDest\" \"$PortTunnel\""
		#CreateTunnel "${PortName}_pf" "$PortDest" "$PortTunnel"
		#echo "CreateTunnel \"${PortName}\" \"$PortDest\" \"$PortTunnel\" \"\" $RAServer 22"
		CreateTunnel "${PortName}" "$PortDest" "$PortTunnel" "" $RAServer 22
	done

	#CreateTunnels_Special
}

CreateTunnels_Special()
{
	local Var PortName PortTunnel PortNameDest PortDest

	for Var in ${!PortNoMon_*}; do
		PortNameDest="${Var#PortNoMon_}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${!Var}"
		#CreateTunnel "${PortName}_NoMon_pf" "$PortDest" "$PortTunnel" "" "" 22 no
		CreateTunnel "${PortName}_NoMon" "$PortDest" "$PortTunnel" "" $RAServer 22 no
	done
}

RemoveTunnels()
{
	local Var PortName PortNameDest

	for Var in ${!Port_*}; do
		PortNameDest="${Var#Port_}"
		PortName="${PortNameDest%_*}"
		#RemoveTunnel "${PortName}_pf"
		RemoveTunnel "${PortName}"
	done

	#RemoveTunnels_Special
	
	/usr/pluto/bin/RA_ChangePassword.sh
}

RemoveTunnels_Special()
{
	local Var PortName PortNameDest

	for Var in ${!PortNoMon_*}; do
		PortNameDest="${Var#PortNoMon_}"
		PortName="${PortNameDest%_*}"
		#RemoveTunnel "${PortName}_NoMon_pf"
		RemoveTunnel "${PortName}_NoMon"
	done
}

DeleteHostKey()
{
	sed -ri '/ra?\.linuxmce\.org/d' /root/.ssh/known_hosts
}

Me="$(basename "$0")"
if [[ "$Me" == "$(basename "$cronCmd")" ]]; then
	DeleteHostKey
	if [[ -n "$remote" ]]; then
		$keepAliveCmd
		AddCronEntry
		[[ "$1" == "restart" ]] && RemoveTunnels
		CreateTunnels
	elif grep -q '^remote=' /etc/pluto.conf; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "$0" "Remote assistance is enabled in pluto.conf, yet remote password is empty. Avoiding closing the tunnels."
	else
		DelCronEntry
		RemoveTunnels
	fi
elif [[ "$Me" == "$(basename "$cronCmd_Special")" ]]; then
	RemoveTunnels_Special
	CreateTunnels_Special
fi
