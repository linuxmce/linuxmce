#!/bin/ash

CONF_FILE="/etc/pluto.conf"

#Remove Commented lines and get options
NumberOfCopies=$((`grep -v "^#" $CONF_FILE | tr -d "\015" | awk -F '=' '/NumberOfCopies/ {print $2}'`))
RotationInterval=$((`grep -v "^#" $CONF_FILE | tr -d "\015" | awk -F '=' '/RotationInterval/ {print $2}'`))

#NumberOfCopies=4
#RotationInterval=$((6 * 60 * 60)) #seconds
if [[ ! -f /tmp/log/pluto/DCERouter.log ]]; then
	exit
fi

Now="$(date +%s)"
if [[ -f /tmp/logrotate_timestamp ]]; then
	LastRotation=$(cat /tmp/logrotate_timestamp)
else
	LastRotation="$Now"
	echo "$Now" >/tmp/logrotate_timestamp
fi
if [[ $((Now - LastRotation)) -lt "$RotationInterval" ]]; then
	exit
fi

for i in /tmp/log/pluto/*.log; do
	Suffix=
	for j in "$i".*; do
		if [[ "$j" == "$i.*" ]]; then
			break;
		fi
		Suffix="$Suffix${j#*.log.} "
	done
	
	#echo "$i - $Suffix"
	
	for S in $(echo "$Suffix" | tr ' ' '\n' | sort -nr); do
		
		S="${S%.gz}"
		
		if [[ "$S" -ge "$NumberOfCopies" ]]; then
			rm -f "$i.$S.gz"
		elif [[ "$S" -ge "1" ]];then
			S1=$((S+1))
			mv "$i.$S.gz" "$i.$S1.gz"
		fi
	done
	
	mv "$i" "$i.1"
	rm -f "$i.1.gz"
	/bin/gzip "$i.1"
	rm -f "$i.1"
	/usr/bin/MessageSend 127.0.0.1 0 -1000 7 10
done
echo "$Now" >/tmp/logrotate_timestamp
