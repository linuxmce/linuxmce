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

Now=$(date +%s)
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
	for S in $(echo "$Suffix" | tr ' ' '\n' | sort -nr); do
		if [[ "$S" -ge "$NumberOfCopies" ]]; then
			rm -f "$i.$S" "$i.$S.gz"
		else
			S1=$((S+1))
			mv "$i.$S" "$i.$S1"
			if [[ "$S1" -gt 1 ]]; then
				gzip -9 "$i.$S1"
			fi
		fi
	done
	mv "$i" "$i.1"
	/usr/bin/MessageSend 127.0.0.1 0 -1000 7 10
done
echo "$Now" >/tmp/logrotate_timestamp
