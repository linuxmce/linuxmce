#!/bin/bash

echo "Marantz AV Receiver Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "@AMT:1\r" -i 1 -s "@AMT:1\r"
if [[ "$?" -ne 0 ]]; then
echo "It's not a Marantz AV Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a Marantz AV Receiver"
# TODO: insert final device id (1914 for now)
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1914
fi
