for dir in /home/* ;do
	if [ -d "$dir/Desktop" ] ;then
		ln -s /usr/share/applications/lmce_launch_manager.desktop "$dir/Desktop/lmce_launch_manager.desktop" || :
		chmod 666 "$dir/Desktop/lmce_launch_manager.desktop" || :
	fi
done

