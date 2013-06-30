#!/bin/bash
echo "Start" > /tmp/move
MediaPluginDeviceID=$1
JobID=$2
TaskID=$3
SRC="$4"
DEST="$5"
DCERouter=dcerouter

rm -f "$DEST"
mv "$SRC" "$DEST"&
# Find out the PID under which the copy process runs.
PID=$!
trap 'kill -KILL $PID &> /dev/null' SIGTERM SIGINT
# Make sure the copy process does not disturb us.
renice -n19 $PID &> /dev/null
# Find out the size of the source.
SRCSIZE=`ls -l "$SRC" | cut -d" " -f 5`
OLDSIZE=0
OLDTIME=`date +%s`
# 
# We need to work with very big numbers to circumvent 
# bash's hate for floats
#
while [ "1" == "`ps -p $PID &> /dev/null && echo 1`" ]
do
	DSTSIZE=`ls -l "$DEST" | cut -d" " -f 5`
	DSTTIME=`date +%s`
	TIMEUSED=$(($DSTTIME - $OLDTIME))
	PERCENTAGE=$((100000000000000 / $SRCSIZE * $DSTSIZE / 1000000000000))	
	if [ "$TIMEUSED" == "0" ]; then
		TIMELEFT=0
	else
		if [ "$PERCENTAGE" != "0" ]; then
			TIMETOTAL=$((( $TIMEUSED * 1000000 / $PERCENTAGE ) / 10000 ))
			TIMELEFT=$(( $TIMETOTAL - $TIMEUSED ))
			/usr/pluto/bin/MessageSend $DCERouter 0 $1 1 1089 9 "Move in progress" 102 "$TIMELEFT" 199 "p" 256 "$PERCENTAGE" 257 "$3" 258 "$2"
		fi
	fi
	
	sleep 1
done
DSTSIZE=`ls -l "$DEST" | cut -d" " -f 5`

if [ "$SRCSIZE" == "$DSTSIZE" ]; then
	/usr/pluto/bin/MessageSend $DCERouter 0 $1 1 1089 9 "Move succeeded" 102 "0" 199 "s" 256 "100" 257 "$3" 258 "$2"
else
   echo "."
fi
