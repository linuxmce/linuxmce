#!/bin/sh
CONF_FILE="/etc/fiire-ra/fiire-ra.conf"
PID_FILE="/var/run/fiire-ra.pid"
SSH_KEY_FILE="/etc/fiire-ra/keys/fiire-ra.key"

PipePath="/var/run/fiire-ra-pipes"
PipeSend="$PipePath/send"
PipeRecv="$PipePath/recv"

SSHConnectionData="fiire-ra@remote.fiire.com"
log() {
	logger -s -t 'fiire-ra' $*
}

InitPipes()
{
	mkdir -p "$PipePath"
	if [[ ! -p "$PipeSend" ]]; then
		rm -rf "$PipeSend"
		mkfifo "$PipeSend"
	fi
	if [[ ! -p "$PipeRecv" ]]; then
		rm -rf "$PipeRecv"
		mkfifo "$PipeRecv"
	fi
}

PortForwarding() {
	while :; do
		ssh -T -y -i "$SSH_KEY_FILE" -R "$PORT:localhost:80" "$SSHConnectionData" <"$PipeSend" >"$PipeRecv" &
		SSHPID=$!
		exec 8>"$PipeSend" 9<"$PipeRecv"

		echo "$USERNAME $PASSWORD_MD5" >&8

		wait "$SSHPID"
		exec 8>&- 9<&-
		log "Connection to fiire server failed, will retry in 60 seconds."
		sleep 60
	done
}

if [[ ! -f "$CONF_FILE" ]]; then
	log "Closing down. Fiire remote access is not configured."
fi

if [[ ! -r "$CONF_FILE" ]] ;then
	log "Failed to read fiire-ra configuration file."
	exit 1
fi

. "$CONF_FILE"

if [[ "$RA_DISABLED" = "1" ]];then
	log "Remote Assistance is disabled. Closing RA..."
	exit 1
fi

PASSWORD_MD5="`echo -n "$PASSWORD" | md5sum | cut -d' ' -f1`"

if [[ -z "$USERNAME" ]] || [[ -z "$PASSWORD" ]] || [[ -z "$PORT" ]]; then
	log "Closing down. Failed to read username and password."
	exit 1
fi

if [[ -z "$PORT" ]] ;then
	log "Closing down. Faild to read assigned port."
	exit 1
fi

if [[ -f "$PID_FILE" ]] ;then
	PID=`cat $PID_FILE`
	if [[ -d /proc/$PID ]] ;then
		log "Another fiire-ra daemon is already running (pid $PID)."
		exit 1
	fi
fi

echo "Starting fiire-ra daemon ..."

InitPipes
exec 0</dev/null
exec 1>/dev/null
exec 2>/dev/null
PortForwarding &
echo $! > "$PID_FILE"
