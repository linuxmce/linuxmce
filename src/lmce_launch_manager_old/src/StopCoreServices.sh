#!/bin/bash

echo "$(date -R) Stopping Core Services" >> /var/log/pluto/StopCoreServices.log
killall UpdateMediaDaemon.sh
killall UpdateMedia

/usr/bin/dcop --all-sessions kded kded loadModule medianotifier

echo "$(date -R) Stopping Core Services done" >> /var/log/pluto/StopCoreServices.log
