#!/bin/bash

export DISPLAY=:0

OSD_Message()
{
	gnome-osd-client --full --dbus "<message id='bootmsg' osd_fake_translucent_bg='off' osd_vposition='center' hide_timeout='1000000' osd_halignment='center'><span foreground='white' font='Arial 72'>$*</span></message>"
}

if [[ -e "/tmp/.X11-unix/X0" ]]; then
	# A little trick to see if Orbiter is Running.

	if [[ -n "$(pidof OrbiterGL)" ]]; then
		OrbiterGLisNotRunning=0
	else
		OrbiterGLisNotRunning=1
	fi

	if [[ -n "$(pidof Orbiter)" ]]; then
		OrbiterIsNotRunning=0
	else
		OrbiterIsNotRunning=1
	fi

	NoOrbitersRunning=$(($OrbiterGLisNotRunning && $OrbiterIsNotRunning))

	if [[ $NoOrbitersRunning -eq 1 ]]; then
		# Orbiter is not running, feedback needed.
		OSD_Message "$@"
	else
		# Orbiter is running, feedback not needed.
		# clear out any messages pending.
		OSD_Message " "
	fi
else
        if [ -x /sbin/usplash_write ]; then
	        usplash_write TEXT-URGENT "$*"
	fi
        if [ -x /bin/plymouth ]; then
                /bin/plymouth update --status="$*"
        fi
 
fi

