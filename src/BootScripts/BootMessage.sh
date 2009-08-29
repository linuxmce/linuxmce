#!/bin/bash

if [ -a "/tmp/.X11-unix/X0" ]
then
	# A little trick to see if Orbiter is Running.
	killall -0 OrbiterGL
	OrbiterGLisNotRunning=$?

	killall -0 Orbiter
	OrbiterIsNotRunning=$?

	NoOrbitersRunning=$OrbiterGLisNotRunning || $OrbiterIsNotRunning


	if [ $NoOrbitersRunning -eq 1 ] 
	then
		# Orbiter is not running, feedback needed.

		export DISPLAY=:0
		gnome-osd-client --full --dbus "<message id='bootmsg' osd_fake_translucent_bg='off' osd_vposition='center' hide_timeout='1000000' osd_halignment='center'><span foreground='white' font='Arial 72'>$*</span></message>"

	else
		# Orbiter is running, feedback not needed.
		# clear out any messages pending.
		export DISPLAY=:0
		gnome-osd-client --full --dbus "<message id='bootmsg' osd_fake_translucent_bg='off' osd_vposition='center' hide_timeout='1000000' osd_halignment='center'><span foreground='white' font='Arial 72'> </span></message>"
	fi

else
	usplash_write TEXT-URGENT "$*"
fi

