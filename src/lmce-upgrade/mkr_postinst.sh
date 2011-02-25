#!/bin/bash
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

## Make a list of hostnames
Q="SELECT p.PK_Device FROM pluto_main.Device p
LEFT JOIN pluto_main.DeviceTemplate p2 ON p2.PK_DeviceTemplate = p.FK_DeviceTemplate
WHERE (p2.FK_DeviceCategory=7 OR p2.FK_DeviceCategory=8) AND p.FK_Device_ControlledVia IS Null AND PK_Device!=1"
UseDB "pluto_main"
moons=$(RunSQL "$Q")
for moon in $moons; do
	# Even if we encounter errors during apt-get update (due to bad connections etc, we carry on)
	chroot /usr/pluto/diskless/$moon apt-get update || /bin/true
	if ping moon$moon -c 1 >/dev/null ; then
		# the moon is up and running, let's update it using the ssh
		rsh moon$moon apt-get dist-upgrade -y		
	else
		# else, run an upgrade in chroot
		chroot /usr/pluto/diskless/$moon apt-get dist-upgrade -y
	fi
done
exit 0