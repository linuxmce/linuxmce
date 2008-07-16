#!/bin/sh

if [[ -n "$(ps | grep -q /usr/bin/DCERouter | grep -v grep)" ]]; then
	exit
fi

mkdir -p /tmp/log/pluto
while [[ ! -f /var/run/pluto.stop ]]; do
	/usr/bin/DCERouter -l /tmp/log/pluto/DCERouter.log
	sleep 1
done
