#!/bin/bash 

. /usr/pluto/bin/Utils.sh

#<-mkr_b_via_b->
exit 0
#<-mkr_b_via_e->
	
AlphaBlending=$(AlphaBlendingEnabled)
OpenGL=$(OpenGLeffects)
cat /etc/X11/xorg.conf | awk -vvAlphaBlending="$AlphaBlending" -vvOpenGL="$OpenGL" -f /usr/pluto/bin/X-CheckSettings.awk
