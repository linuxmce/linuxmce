#!/bin/bash

REPORT_FILE=/tmp/BluetoothDongle_BugReport	# The file to save the report in
TIME_INTERVAL=2 				# No of hours between sending reports
MAIL_ADDRESS="bugs@monsternet.us"		# Mail address to send bugs to

## Check to see if $TIME_INTERVAL hours passed since last report
LastReport=$(stat -c %Y $REPORT_FILE 2>/dev/null)
HourAgo=$(( $(date +%s) - 60*60*$TIME_INTERVAL  ))

if [[ "$LastReport" != "" && $LastReport > $HourAgo ]] ;then
	echo "Don't send report now, didn't pass a hour since last time"
	exit 0
fi

## Backup the old report and create the new one
mv $REPORT_FILE $REPORT_FILE.$TIME_INTERVAL
touch $REPORT_FILE

## Write our report
echo "*date -R*"			>> $REPORT_FILE
date -R 				>> $REPORT_FILE
echo -e "\n\n\n"			>> $REPORT_FILE

echo "*ifconfig*"			>> $REPORT_FILE
ifconfig				>> $REPORT_FILE
echo -e "\n\n\n"			>> $REPORT_FILE

echo "*cat /etc/pluto.conf*"		>> $REPORT_FILE
cat /etc/pluto.conf			>> $REPORT_FILE
echo -e "\n\n\n"			>> $REPORT_FILE

echo "*dmesg*"				>> $REPORT_FILE
dmesg 					>> $REPORT_FILE
echo -e "\n\n\n"			>> $REPORT_FILE

## Send us a mail with the report
cat $REPORT_FILE | mail -s "[BLUETOOTH DONGLE] Bug Report" $MAIL_ADDRESS
