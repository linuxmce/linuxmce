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
rm -f /etc/rc5.d/{S99core,S99launch-manager}

# Make sure the new startup scripts are being used
#ln -sfv /usr/pluto/bin/Start_X.sh /etc/rc5.d/S30Start_X.sh
#ln -sfv /usr/pluto/bin/startup-script.sh /etc/rc5.d/S93startup-script.sh
#ln -sfv /usr/pluto/bin/lmce_launch_manager.sh /etc/rc5.d/S99lmce_launch_manager.sh
rm -f /usr/pluto/bin/Start_X.sh /etc/rc5.d/S30Start_X.sh
rm -f /usr/pluto/bin/startup-script.sh /etc/rc5.d/S93startup-script.sh
rm -f /usr/pluto/bin/lmce_launch_manager.sh /etc/rc5.d/S99lmce_launch_manager.sh

ln -sfv /etc/init.f/linuxmce /etc/rc5.d/S93linuxmce