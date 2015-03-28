#!/bin/bash

# setup the interactor and the firstboot script
update-rc.d -f 90_start_interactor.sh start 90 2 3 4 5 . >/dev/null
update-rc.d -f firstboot start 91 2 3 4 5 . >/dev/null

###########################################################
### Setup global variables
###########################################################

MD_Preseed () {
	update-rc.d -f kdm remove
	update-rc.d -f lightdm remove
	update-rc.d -f NetworkManager remove

	cat <<-EOF > /etc/network/interfaces
		auto lo
		iface lo inet loopback

		auto eth0
		iface eth0 inet dhcp
		EOF

	echo '/bin/false' >"/etc/X11/default-display-manager"
}



###########################################################
### Main execution area
###########################################################

#Function execution
MD_Preseed

#Exit successfully
exit 0
