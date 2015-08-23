#!/bin/bash

if [ install = "$1" ]; then
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
