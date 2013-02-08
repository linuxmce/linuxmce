#!/bin/bash

/etc/init.d/vdr stop    
sleep 5

echo 'Going to copy channels list file : '."$1"

cp -v "$1" /var/lib/vdr/channels.conf
chown vdr.vdr /var/lib/vdr/channels.conf

/etc/init.d/vdr start

