#!/bin/sh
#########

# Post-Install for Activate Orbiter Plasmoid.
# Automatically Attempts to add the Orbiter Plasmoid to 
# The Desktop, while refreshing the list of widgets. 

cat >/root/.kde/share/config/plasma-appletsrc <<"EOF"
[Containments][1]
backgroundmode=0
selected=
wallpaper=/usr/share/wallpapers/Blue_Curl/contents/images/1280x800.jpg
wallpapercolor=224,223,222
wallpaperposition=2

[Containments][1][Applets][13]
geometry[$d]
immutability[$d]
plugin[$d]
zvalue[$d]

[Containments][1][Applets][13][Configuration]
autoSave[$d]

[Containments][1][Applets][16]
geometry=0,0,200,200
immutability=1
plugin=plasma_applet_activateorbiter
zvalue=0

EOF

# Rebuild the System Database. 
# /usr/bin/kbuildsycoca

