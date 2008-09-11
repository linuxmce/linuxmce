 #!/bin/bash
# Play and Play script for Olevia TV's Models Numbers:2XXT/5XXT Possibly more
# Power Up is very slow so it requires several seconds to get feedback
# Device Template Number:1960

# The TV only responds to the "Power Status Query" command when the power is
# on, so we'll send the Power On command first, then sent the Query command.
# We give it a delay to allow the TV to power up
echo "Olevia TV Detection script queue $2"
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 115200 -t "\BE\05\27\01\EB" -i 7
echo "Waiting for powerup"

# Now query to check for the TV power status reply
/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 115200 -t "\BE\05\90\00\53" -i 1 -s "\06\05\90\01\9C"
if [[ "$?" -ne 0 ]]; then
	echo "It's not an Olevia TV"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 0
else
	echo "It is a Olevia TV"
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 1960
fi