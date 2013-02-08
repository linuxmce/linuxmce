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
rm -f /etc/rc{2,3,4,5}.d/{S30Start_X.sh,S93startup-script.sh,S99lmce_launch_manager.sh,S93linuxmce,S99core,S99launch-manager}
rm -f /etc/rc2.d/S99kdm

# Add a single new startup script.
ln -sfv /etc/init.d/linuxmce /etc/rc2.d/S99linuxmce
ln -sfv /etc/init.d/linuxmce /etc/rc5.d/S99linuxmce
