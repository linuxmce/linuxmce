#!/bin/bash

echo "JVC Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\21\89\01\00\00\0A" -i 1 -s "\06\89\01\00\00\0A"
if [[ "$?" -ne 0 ]]; then
echo "It's NOT a JVC D-ILA Projector"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It's a JVC D-ILA Projector"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2162
fi
