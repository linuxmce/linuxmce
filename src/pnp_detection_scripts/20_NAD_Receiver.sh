#!/bin/bash

echo "NAD  Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 115200 -t "\rMain.Model?\r" -i 1 -s "Main.Model=T765\r"
if [[ "$?" -ne 0 ]]; then
echo "It's not a NAD Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is a NAD Receiver"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1976
fi
