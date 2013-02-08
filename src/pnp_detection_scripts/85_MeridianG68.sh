#!/bin/bash

echo "Meridian G68 script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "DV\r" -i 3 -s "DVD "
#don't search for something we send because anything that echoes will be a match
if [[ "$?" -ne 0 ]]; then
echo "It's not a Meridian G68"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Meridian G68"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1973
fi
