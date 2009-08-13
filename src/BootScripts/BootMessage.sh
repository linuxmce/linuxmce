#!/bin/bash

if [ -a "/tmp/.X11-unix/X0" ]
then
	export DISPLAY=:0
	gnome-osd-client --full --dbus "<message id='bootmsg' osd_fake_translucent_bg='off' osd_vposition='center' hide_timeout='1000000' osd_halignment='center'><span foreground='white' font='Arial 72'>$*</span></message>"
else
	usplash_write TEXT-URGENT "$*"
fi

usplash_write TEXT-URGENT "$*"
