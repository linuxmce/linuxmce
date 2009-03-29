#!/bin/bash

#create launcher icons
for dir in /home/* ;do
	if [ -d "$dir/Desktop" ] ;then
		ln -s /usr/share/applications/lmce_launch_manager.desktop "$dir/Desktop/lmce_launch_manager.desktop" || :
		chmod 666 "$dir/Desktop/lmce_launch_manager.desktop" || :
	fi
done

#Temporarily disable startCore.sh cleancore script for 3-29-2009 build LM testing.
#Backup the original, then replace it with a "neutered" version
if [ -f /usr/pluto/bin/startCore.sh ] ; then
	mv /usr/pluto/bin/startCore.sh /usr/pluto/bin/startCore.sh._backup
	cat >/usr/pluto/bin/startCore.sh <<"EOF"
#!/bin/sh

exit 0
EOF
	chmod +x /usr/pluto/bin/startCore.sh
fi

