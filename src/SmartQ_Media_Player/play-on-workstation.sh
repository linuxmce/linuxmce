#!/bin/sh

FILETOPLAY=$@

TRANSLATEDFILETOPLAY=`echo $FILETOPLAY | sed -e "s/\/mnt\//\//"`
echo $TRANSLATEDFILETOPLAY

/home/thomas/workstation-work/MessageSend dcerouter 0 10 1 43 2 "" 13 "$TRANSLATEDFILETOPLAY" 29 "5" 44 "" 45 "6" 116 "" 117 "" 253 "" 254 "" 276 "" 
