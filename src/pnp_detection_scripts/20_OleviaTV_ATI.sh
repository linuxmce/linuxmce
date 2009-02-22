#!/bin/bash
#Play and Play script for Olevia TV's Models Numbers:2XXT/5XXT Possibly more
#Power Up is very slow so it requires several seconds to get feedback
#Device Template Number:1917 on my local Machine.

#The TV only responds to the "Power Status Qery" command when the power is on, so we'll send the Power On command first, then sent the Query command.
#We give it a delay to allow the TV to power up
echo "Olevia TV (ATI) Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 115200 -t "\BE\05\27\01\EB" -i 8
echo "Waiting for powerup"
#Now query to check for the TV power status reply
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 115200 -t "\BE\05\90\00\53" -i 1 -s "\06\05\90\01\9C"
if [[ "$?" -ne 0 ]]; then
echo "It's not an Olevia TV (ATI)"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
echo "It is an Olevia TV (ATI) Template #1950"
/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1950
fi