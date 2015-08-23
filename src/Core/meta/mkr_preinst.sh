#!/bin/bash

if [ install = "$1" ]; then
	if [ $(/bin/hostname) != "dcerouter" ] ; then
		## Setup /etc/hosts
		cat <<-EOF >/etc/hosts
			127.0.0.1 localhost.localdomain localhost
			127.0.1.1 dcerouter $(/bin/hostname)
			EOF
	fi

	# Put a temporary nis config file that will prevent ypbind to start
	# Temporary NIS setup, disabling NIS server and client.
	cat <<-EOF >/etc/default/nis
		NISSERVER=false
		NISCLIENT=false
		YPPWDDIR=/etc
		YPCHANGEOK=chsh
		NISMASTER=
		YPSERVARGS=
		YPBINDARGS=
		YPPASSWDDARGS=
		YPXFRDARGS=
		EOF
fi

exit 0
