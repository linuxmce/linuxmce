#!/bin/ash

cronCmd="/usr/bin/SetupRemoteAccess.sh"
cronCmd_Special="/usr/bin/SetupRA-Special.sh"
cronEntry="*/1 * * * * ash -c '$cronCmd >/dev/null 2>&1' #RA_std"
cronEntry_Special="*/10 * * * * ash -c '$cronCmd_Special >/dev/null 2>&1' #RA_special"

UpdateInterval=86400 #seconds
RaPass=$(cat /etc/pluto/ra_password 2>/dev/null)

log_file="/tmp/log.setupRA"

log() {
	#logger -s -t 'SetupRA' $*
    echo "`date` - $*" >> $log_file
}

log "===Start SetupRemoteAccess script==="

GetPorts()
{
    log "=GetPorts="
	Result=$(cat /etc/pluto/ra_ports 2>/dev/null)
	case "$Result" in
		*';'*) : ;;
		*) Result="$Result;time=0" ;;
	esac
	Ports="${Result%%;*}"
	Vars="${Result#*;}"
}

GetPorts
OldIFS="$IFS"
IFS=,
for Var in $Vars; do
	case "$Var" in
		time=*)
			LastUpdate="${Var#time=}"
			Now="$(date +%s)"
			if expr $Now - $LastUpdate '<' 0 >/dev/null || expr $Now - $LastUpdate '>' $UpdateInterval >/dev/null; then
				/usr/bin/RA-handler.sh --confirm
				GetPorts
			fi
		break
		;;
	esac
done

for Port in $Ports; do
	case "$Port" in
		nomon_*)
			VarsPortNoMon="${VarsPortNoMon} PortNoMon_${Port#nomon_}"
			;;
		*)
			VarsPortStd="${VarsPortStd} Port_$Port"
			;;
	esac
done
IFS="$OldIFS"

if [[ ! -L "$cronCmd_Special" ]]; then
	rm -rf "$cronCmd_Special"
	ln -s "$cronCmd" "$cronCmd_Special"
fi

RAKey="/etc/pluto/ra_key"
[[ -f "$RAKey" ]] && chmod 700 "$RAKey" || exit


AddCronEntry()
{
    log "=AddCronEntry="
	local CronReload
	
	# if this script is not in a cron job
	if ! grep -qF "#RA_std" /etc/crontabs/root 2>/dev/null; then
		# add it to crontab
		echo "$cronEntry" >>/etc/crontabs/root
		CronReload=1
		log "Added crontab entry"
	else
		log "Crontab entry already present. Not adding."
	fi

	if ! grep -qF "#RA_special" /etc/crontabs/root; then
		# add it to crontab
		echo "$cronEntry_Special" >>/etc/crontabs/root
		CronReload=1
		log "Added crontab entry (special)"
	else
		log "Crontab entry (special) already present. Not adding."
	fi
	
	if [[ -n "$CronReload" ]]; then
		/etc/init.d/cron restart
	fi
}

DelCronEntry()
{
    log "=DelCronEntry="
	local CronReload

	# remove script from crontab
	if grep -qF "#RA_std" /etc/crontabs/root; then
		sed -ir '/#RA_std/d' /etc/crontabs/root
		CronReload="1"
		log "Crontab entry found. Removed."
	else
		log "Crontab entry not found. Not removing."
	fi
	
	if grep -qF "#RA_special" /etc/crontabs/root; then
		sed -ir '/#RA_special/d' /etc/crontabs/root
		CronReload="1"
		log "Crontab entry (special) found. Removed."
	else
		log "Crontab entry (special) not found. Not removing."
	fi

	if [[ -n "$CronReload" ]]; then
		/etc/init.d/cron restart
	fi
}

CreateTunnel()
{
    log "=CreateTunnel"
	Suffix="$1"
	LocalPort="$2"
	RemotePort="$3"
	Host="$4"
	RAhost="${5:-cisco.fiire.com}"
	RAport="$6"
	Monitored="${7:-yes}"

	Tunnel=$(cat /tmp/tunnel.$Suffix 2>/dev/null)
	if [[ -n "$Tunnel" ]] && [[ ! -d /proc/"$Tunnel" ]]; then
		rm -f /tmp/tunnel.$Suffix
		Tunnel=
	fi

	# if tunnel is not active create it
	Dead=0
	if [[ "$Monitored" == yes ]] && [[ "$RA_CheckRemotePort" == "1" ]]; then
		if [[ -z "$Tunnel" ]] || ! /usr/bin/NetworkTest "$RAhost" "$RemotePort"; then
			Dead=1
		fi
	else
		if [[ -z "$Tunnel" ]]; then
			Dead=1
		fi
	fi
	if [[ "$Dead" -eq 1 ]]; then
		FalseAlarm=0
		if [[ -n "$Tunnel" ]] && [[ "$RA_CheckRemotePort" == "1" ]] && [[ "$Monitored" == yes ]]; then
			for i in $(seq 1 5); do
				/usr/bin/NetworkTest "$RAhost" "$RemotePort" >/dev/null 2>&1 && FalseAlarm=1 && break
				sleep 1
			done
		fi
		if [[ "$FalseAlarm" -eq 0 ]]; then
			[[ -n "$Tunnel" ]] && RemoveTunnel "$Suffix"
			/usr/bin/RemoteAccess_Tunnel.sh "$RemotePort" "$LocalPort" "$RAKey" "$Host" "$RAhost" "$RAport" &
			echo $! >/tmp/tunnel.$Suffix
			log "$Suffix tunnel enabled."
		else
			log "$Suffix tunnel not down. False alarm"
		fi
	else
		log "$Suffix tunnel already present. Not enabling."
	fi
}

RemoveTunnel()
{
    log "=RemoveTunnel="
	Suffix="$1"
	Tunnel=$(cat /tmp/tunnel.$Suffix 2>/dev/null)
	if [[ -n "$Tunnel" ]] && [[ ! -d /proc/"$Tunnel" ]]; then
		rm -f /tmp/tunnel.$Suffix
		Tunnel=
	fi

	# kill port forward if it exists
	if [[ -n "$Tunnel" ]]; then
		AllTunnels=$(ps|awk '/RemoteAccess_Tunnel.sh/ && !/awk/ {print $1}')
		for PID in $AllTunnels; do
			PPID=$(awk '/^PPid:/ {print $2}' /proc/$PID/status)
			if [[ "$PPID" == "$Tunnel" ]]; then
				kill "$PID"
				break
			fi
		done
		kill $Tunnel
		log "$Suffix tunnel found. Closed."
	else
		log "$Suffix tunnel not found. Not closing."
	fi
}

CreateTunnels()
{
    log "=CreateTunnels="
	/usr/bin/RA_ChangePassword.sh

	local Var PortName PortTunnel PortNameDest PortDest

	for Var in $VarsPortStd; do
		PortNameDest="${Var#Port_}"
		PortNameDest="${PortNameDest%=*}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${Var#*=}"
		CreateTunnel "${PortName}_pf" "$PortDest" "$PortTunnel"
	done

	CreateTunnels_Special
}

CreateTunnels_Special()
{
    log "=CreateTunnels_Special="
	local Var PortName PortTunnel PortNameDest PortDest

	for Var in $VarsPortNoMon; do
		PortNameDest="${Var#PortNoMon_}"
		PortNameDest="${PortNameDest%=*}"
		PortName="${PortNameDest%_*}"
		PortDest="${PortNameDest##*_}"
		PortTunnel="${Var#*=}"
		CreateTunnel "${PortName}_NoMon_pf" "$PortDest" "$PortTunnel" "" "" 22 no
	done
}

RemoveTunnels()
{
    log "=RemoveTunnels="
	local Var PortName PortNameDest

	for Var in $VarsPortStd; do
		PortNameDest="${Var#Port_}"
		PortName="${PortNameDest%_*}"
		RemoveTunnel "${PortName}_pf"
	done

	RemoveTunnels_Special
	
	/usr/bin/RA_ChangePassword.sh
}

RemoveTunnels_Special()
{
    log "=RemoveTunnels_Special="
	local Var PortName PortNameDest

	for Var in $VarsPortNoMon; do
		PortNameDest="${Var#PortNoMon_}"
		PortName="${PortNameDest%_*}"
		RemoveTunnel "${PortName}_NoMon_pf"
	done
}

DeleteHostKey()
{
    log "=DeleteHostKey="
	sed -ri '/cisco\.fiire\.com/d' /root/.ssh/known_hosts
}

Me="$(basename "$0")"

if [[ "$Me" == "$(basename "$cronCmd")" ]]; then
    log "===$Me==="
	DeleteHostKey
	if [[ -n "$RaPass" ]]; then
		/usr/bin/RA_KeepPortsAlive.sh
		AddCronEntry
		[[ "$1" == "restart" ]] && RemoveTunnels
		CreateTunnels
	elif [[ -f /etc/pluto/ra_password ]]; then
		log "Remote assistance is enabled in pluto.conf, yet remote password is empty. Avoiding closing the tunnels."
	else
		DelCronEntry
		RemoveTunnels
	fi
elif [[ "$Me" == "$(basename "$cronCmd_Special")" ]]; then
    log "===$Me==="
	RemoveTunnels_Special
	CreateTunnels_Special
fi

log "===End SetupRemoteAccess script==="
