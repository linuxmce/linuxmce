#!/bin/bash

if [ install = "$1" ]; then
	#Seeding mythweb preferences to not override the LMCE site on install - for some odd reason, mythweb packages don't accept the set-selections
	touch /etc/default/mythweb
	cat <<-EOF >/etc/default/mythweb
		[cfg]
		enable = false
		only = false
		username =
		password =
		EOF
fi
