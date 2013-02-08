#!/bin/bash

# RemotePort, LocalPort, RAKey, Host, RAhost, RAport

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh

if [[ "$#" -lt 3 || "$#" -gt 6 ]]; then
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Called with wrong number of parameters: $#"
	exit 1
fi

RemotePort="$1"
LocalPort="$2"
RAKey="$3"
Host="$4"
RAhost="$5"

if [[ -z "$RAhost" ]]; then
	RAhost="ra.linuxmce.org"
fi

if [[ -n "$6" ]]; then
	RAport="$6"
fi

if [[ -z "$RAport" || "${RAport//[0-9]}" != "" ]]; then
	RAport=80
fi

if [[ -z "$Host" ]]; then
	Host=localhost
fi

if [ -f "$RAKey" ]; then
	chmod 700 "$RAKey"
	chown root.root "$RAKey"
else
	Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Key '$RAKey' not found. Exiting"
	exit 1
fi
# TODO: check or remove host key every time just in case
# TODO: check if port 80 isn't proxyed and if it is, also check port 22
while echo 'x'; do
	sleep 5
done |
	ssh -p $RAport -i $RAKey -R$RemotePort:$Host:$LocalPort "remoteassistance@$RAhost" |
	tee /dev/tty | /usr/pluto/bin/charperiod
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Tunnel to port $LocalPort died"
