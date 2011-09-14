#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Utils.sh	

# if user doesn't exist
if [[ -n "$remote" ]]; then
	Pass=$(openssl passwd -1 -salt 'RAlmcePW' "$remote")
	if ! grep -q '^remote:' /etc/passwd; then
		# add user "remote" w/ the password specified (var value)
		useradd -u 0 -g 0 -o -d /root -s /bin/bash -c "LinuxMCE Remote Assistance Account" -p "$Pass" remote || Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Failed to add user 'remote' with exit code '$?'"
		SCRvar="export SCREENDIR=/var/run/screen/S-root"
		grep -qF "$SCRvar" /root/.bashrc || echo "$SCRvar" >>/root/.bashrc
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "User 'remote' already exists. Not adding."
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Setting password for 'remote' user"
		if ! BlacklistConfFiles '/etc/shadow' ;then
			if [ ! -e /etc/shadow.pbackup ] ;then
				cp /etc/shadow /etc/shadow.pbackup
			fi	
			sed -i '/^remote:/ s/^\([^:]*\):[^:]*:\(.*\)$/\1:'"${Pass//\//\/}"':\2/' /etc/shadow
		fi
	fi
else
	if grep -q remote /etc/passwd; then
		# delete user "remote" if it exists
		userdel remote || Logging "$TYPE" "$SEVERITY_WARNING" "$0" "Failed to remove user 'remote' with exit code '$?'"
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "User 'remote' not present. Nothing to remove"
	fi
fi
