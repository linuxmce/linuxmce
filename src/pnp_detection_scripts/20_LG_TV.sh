#!/bin/bash
echo "LG TV detection script queue $2"
# make sure its on
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "ka 01 01\r\n" -s "a 01 OK" -i 2
# double check that its on
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "ka 01 FF\r\n" -s "a 01 OK" -i 2

if [[ "$?" -ne 0 ]]; then
echo "It's not a LG TV"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
exit
fi
# older TVs should not have a "xb" command
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "xb 01 FF\r\n" -s "b 01 OK" -i 2
if [[ "$?" -eq 0 ]]; then
echo "LG TV with newer command set"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2126
exit
fi
# double check that it responds correctly to the older command
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "kb 01 FF\r\n" -s "b 01 OK" -i 2
if [[ "$?" -eq 0 ]]; then
echo "LG TV with older command set"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1915
fi

