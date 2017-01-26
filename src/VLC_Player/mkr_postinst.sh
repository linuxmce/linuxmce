#!/bin/bash

# we create a vlcblack.mpeg file to provide a black screen 
# in case nothing is there to show.

if [ ! -f /usr/pluto/share/vlcblack.mpeg ] ; then
	TMPFILE=`tempfile -s ".png"`
	convert -size 640x480 xc:black $TMPFILE
	ffmpeg -loop 1 -i $TMPFILE -t 900 -r 1 -c:v libx264 /usr/pluto/share/vlcblack.mpeg
fi