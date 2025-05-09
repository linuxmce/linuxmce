#!/bin/bash

. /tmp/mce_wizard_data.sh
. ./mce-installer-common.sh

Setup_Apt_Conffiles

apt-get -y install libdvdread3 || exit 1
/usr/share/doc/libdvdread3/install-css.sh &
wait_pid=$!

## Wait for 100 seconds to finish
end_date=$(( $(date +%s) + 100 ))
while [[ -d /proc/$wait_pid ]] ;do
	sleep 0.5
	if [[ "$end_date" -gt "$(date +%s)" ]] ;then
		echo "Waiting for dvdcss to be installed ..."
	else
		echo "Operation timed out"
		kill "$wait_pid"
		exit 1
	fi
done

exit 0
