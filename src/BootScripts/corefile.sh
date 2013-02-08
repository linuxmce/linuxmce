#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

Kver=$(uname -r)
Kmaj=$(echo "$Kver" | cut -d. -f1)
Kmin=$(echo "$Kver" | cut -d. -f2)

echo "# set this to 0 to disable apport
enabled=0

# set maximum core dump file size (default: 209715200 bytes == 200 MB)
maxsize=209715200" >/etc/default/apport

if [[ "$Kmaj.$Kmin" == "2.6" || "$Kmaj" == "3" ]]; then
	if [[ ! -d /home/coredump/$PK_Device ]] ;then
		mkdir -p "/home/coredump/$PK_Device"
	fi

	rm -f /usr/pluto/coredump	
	ln -s "/home/coredump/$PK_Device" /usr/pluto/coredump

	echo "/home/coredump/$PK_Device/core_%e_%t_%s" >/proc/sys/kernel/core_pattern
elif [ "$Kmaj.$Kmin" == "2.4" ]; then
	echo 1 >/proc/sys/kernel/core_uses_pid
else
	echo "Not setting core settings. Unknown kernel version: '$Kmaj.$Kmin'"
fi
