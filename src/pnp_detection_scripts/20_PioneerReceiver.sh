#!/bin/bash

echo "Pioneer Detection script queue $2"

/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\r\s500m\rPO\r" -i 4 -s "PWR0"
if [[ "$?" -ne 0 ]]; then
	echo "It's not a Pioneer Receiver" 
	/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13  "$4" 44 0
else
	echo "Checking Model"
	/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "\r\s500m\rBPO\r" -i 4 -s "BPR0" # Send power to Zone 3
		if [[ "$?" -ne 0 ]]; then
			echo "It is a Pioneer Elite VSX-21TXH Receiver"
			/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2033
		else
			/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "26FN\r" -i 4 -s "FN26" # Change input to Home Media Gallery
			if [[ "$?" -ne 0 ]]; then
				echo "It is a Pioneer Elite VSX-23TXH Receiver"
				/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2034
			else
				/usr/pluto/bin/TestSerialPort -p $3 -P N81 -b 9600 -t "0055SR\r" -i 4 -s "SR0055" # Change listerning Mode to THX Select2 Games
				if [[ "$?" -ne 0 ]]; then
					echo "It is a Pioneer Elite SC-27 Receiver"
					/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2035
				else
					echo "It is a Pioneer Elite SC-25 Receiver"
					/usr/pluto/bin/MessageSend dcerouter -r 0 $1 1 806 224 $2 13 "$4" 44 2036
				fi
			fi
		fi
fi

