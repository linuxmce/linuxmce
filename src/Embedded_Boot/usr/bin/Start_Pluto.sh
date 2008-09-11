#!/bin/sh

case "$1" in
	xdebug) set -x ;;
	debug) : ;;
	*) exec >/dev/null 2>/dev/null </dev/null ;;
esac

rm -f /var/run/pluto.stop

/usr/bin/Start_DCERouter.sh &

Devices=
while [[ -z "$Devices" ]]; do
	Devices=$(MessageSend 127.0.0.1 -targetType template -o 0 27 1 956 2 1 | awk '/^5:/,/^$/' | sed -r 's/^5://' | grep -v DCERouter)
	sleep 1
done

OldIFS="$IFS"
IFS="|"
echo "$Devices" | while read dev desc cmd rest; do
	if [[ -z "$cmd" ]]; then
		cmd=$(echo "$desc" | sed -r 's,[ /],_,g')
	fi
	if [[ -x /usr/bin/"$cmd" ]]; then
		/usr/bin/Start_Device.sh "$dev" "$cmd" &
	fi
done
IFS="$OldIFS"
