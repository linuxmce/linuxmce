#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

Q="SELECT COUNT(*) FROM Users"
R=$(RunSQL "$Q")

if [[ "$R" != "0" ]] ; then
	echo "$(date -R) SetupUsers" >> /var/log/pluto/SetupUsers.log
	cd /usr/pluto/bin
	bash -x ./SetupUsers_Homes.sh >> /var/log/pluto/SetupUsers.log  2>&1
	if [ -x /usr/pluto/bin/generate_users_voicemenu.pl ]
	then
		./generate_users_voicemenu.pl
	fi
fi
