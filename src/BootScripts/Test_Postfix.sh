#!/bin/bash 

mailFromName=$1 
mailFromAddress=$2 
mailTo=$3

# test actual postfix config.
echo "Sending test email..." 
mailx -r "$mailFromName <$mailFromAddress>" -s "Email test from LinuxMCE" $mailTo < /etc/hosts 
sleep 7 
	 
status=$(tail -3 /var/log/mail.log | grep status | sed -n 's/^.*status=\(.*\).*$/\1/p')

if [[ $(echo $status | cut -c -4) == "sent" ]]; then
	echo "Success: $status"
	exitcode=0
else
	echo "Error: $status"
	exitcode=1
fi


exit $exitcode
