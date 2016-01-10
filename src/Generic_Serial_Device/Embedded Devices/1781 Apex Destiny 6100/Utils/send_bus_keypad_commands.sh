#!/bin/bash

echo sending commands to $1

/usr/pluto/bin/TestSerialPort -p $1 -P N81 -b 1200 -t "12zk10300009090062\r\n" -i 0
/usr/pluto/bin/TestSerialPort -p $1 -P N81 -b 1200 -t "12zk10300001010072\r\n" -i 0
/usr/pluto/bin/TestSerialPort -p $1 -P N81 -b 1200 -t "12zk10300007070066\r\n" -i 0
/usr/pluto/bin/TestSerialPort -p $1 -P N81 -b 1200 -t "12zk1030000303006E\r\n" -i 0
