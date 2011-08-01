#!/bin/bash

echo "Oppo BDP Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "#QPW\0d" -i 1 -s "@OK"
if [[ "$?" -ne 0 ]]; then
echo "It's NOT a Oppo BDP"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It's a Oppo BDP"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2160
fi
