#!/bin/bash

# we create a vlcblack.mpeg file to provide a black screen 
# in case nothing is there to show.

if [ ! -f /usr/pluto/share/vlcblack.mpeg ] ; then
	TMPFILE=`tempfile -s ".png"`
	convert -size 640x480 xc:black $TMPFILE
	cp $TMPFILE /usr/pluto/share/vlcblack.mpeg
fi