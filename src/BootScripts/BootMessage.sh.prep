#!/bin/bash

#<-mkr_b_ubuntu_b->
[[ -z "$(pidof mce-launcher)" ]] || echo "%$*" > /tmp/mce-launcher-pipe
exit 0
#<-mkr_b_ubuntu_e->

## Check if we are in silent mode else exit
grep -q "silent" /proc/splash 2>/dev/null || exit 0

## Dummy function so we can parse the bootsplash config on the fly
function box() { true; }

## Parse the bootsplash configuration file
test -f "/etc/bootsplash/themes/current/config/bootsplash-`/sbin/fbresolution`.cfg" && . /etc/bootsplash/themes/current/config/bootsplash-`/sbin/fbresolution`.cfg

## Read the current progress for /var/run/bootsplash_progress
if [[ -r /var/run/bootsplash_progress ]] ;then
	progress=$(cat /var/run/bootsplash_progress)
fi
	
## Redraw the progress bar (also clears previous text)
echo "show $(( $progress + 1 ))" > /proc/splash
echo "show $progress" > /proc/splash

## Draw the text on the progressbar
if [ "$text_x" != "" -a "$text_y" != "" -a "$text_color" != "" -a "$text_size" != "" ] ;then
	text_font="/etc/bootsplash/fonts/VeraBd.ttf"
	/sbin/fbtruetype -x$(($text_x + 1)) -y$(($text_y + 1)) -t000000 -s$text_size -f$text_font "$*"
	/sbin/fbtruetype -x$text_x -y$text_y -t$text_color -s$text_size -f$text_font "$*"
fi
