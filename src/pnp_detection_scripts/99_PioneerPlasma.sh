#!/bin/bash

echo "Pioneer Plasma script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 4800 -t "\0201AMN\03" -i 1 -s "\02XXX" -s "\02AMN"
#don't search for something we send because anything that echoes will be a match
if [[ "$?" -ne 0 ]]; then
echo "It's not a Pioneer Plasma"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Pioneer Plasma"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1779
fi
