#!/bin/bash

#create launcher icons

for dir in /home/* ;do
	if [ -d "$dir/Desktop" ] ;then
		if [ -f "$dir/Desktop/lmce_launch_manager.desktop" ]; then
			# The current lmce_launch_manager is a CLI app.
			rm "$dir/Desktop/lmce_launch_manager.desktop"
		fi
#		ln -s /usr/share/applications/lmce_launch_manager.desktop "$dir/Desktop/lmce_launch_manager.desktop" || :
#		chmod 666 "$dir/Desktop/lmce_launch_manager.desktop" || :
	fi
done

# Make sure the old startup scripts no longer exists
#rm -f /etc/rc{2,3,4,5}.d/{S30Start_X.sh,S93startup-script.sh,S99lmce_launch_manager.sh,S93linuxmce,S99core,S99launch-manager}
#rm -f /etc/rc2.d/S99kdm
update-rc.d -f Start_X.sh remove
update-rc.d -f startup-script.sh remove
update-rc.d -f lmce_launch_manager.sh remove
update-rc.d -f core remove
update-rc.d -f launch-manager remove
