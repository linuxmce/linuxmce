#!/bin/bash

EXT=
#<-mkr_b_ubuntu_b->
EXT=".pluto"
#<-mkr_b_ubuntu_e->

awk '/Section..*"Files"/,/EndSection/ { if ($1 == "ModulePath") print "\tModulePath\t\"/usr/lib/xorg/modules\""; else print; next } { print }' /etc/X11/xorg.conf.pluto"$EXT" >/etc/X11/xorg.conf.pluto.$$
mv /etc/X11/xorg.conf.pluto{.$$,"$EXT"}
