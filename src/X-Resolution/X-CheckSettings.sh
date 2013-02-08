#!/bin/bash 

. /usr/pluto/bin/Utils.sh

#
	
AlphaBlending=$(AlphaBlendingEnabled)
OpenGL=$(OpenGLeffects)
cat /etc/X11/xorg.conf | awk -vvAlphaBlending="$AlphaBlending" -vvOpenGL="$OpenGL" -f /usr/pluto/bin/X-CheckSettings.awk
