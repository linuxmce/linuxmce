#
# Regular cron jobs for the lmce-screen-capture-camera package
#
0 4	* * *	root	[ -x /usr/bin/lmce-screen-capture-camera_maintenance ] && /usr/bin/lmce-screen-capture-camera_maintenance
