#!/bin/bash

USAGE="Usage: `basename $0` enable|disable username"

secrets=/etc/ppp/chap-secrets

enable()
{
	echo "Enabling VPN for user $1"
	
	# check if user does not already have VPN access
	grep -w ^$1 $secrets >/dev/null
	if [[ "$?" -ne "0" ]]; then
		# check if user maybe exists but is commented out
		grep -w ^#$1 $secrets >/dev/null
		if [[ "$?" -eq "0" ]]; then
			echo "$1 already exists in secrets file, enabling access"
			sed -i "s/^#$1/$1/" $secrets
		else
			echo "$1 does not exist, creating new access"
			echo "$1	l2tpd	$2	*" >>$secrets
		fi
	else
		echo "$1 already has VPN access, nothing to do"
	fi
}

disable()
{
	echo "Disabling VPN for user $1"

	# check if user really has VPN access
	grep -w ^$1 $secrets >/dev/null
	if [[ "$?" -eq "0" ]]; then
		echo "$1 has enabled VPN access, disabling access"
		sed -i "s/^$1/#$1/" $secrets
	else
		echo "$1 does not have VPN access enabled, nothing to do"
	fi
	
	#
}

password()
{
	echo "Setting new Password for user $1"
	awk "{if (\$1 == \"$1\" && \$3 == \"$2\") sub(/$2/,\"$3\"); print \$0}" <$secrets >$secrets.bak
	if [[ "$?" -eq "0" ]]; then
		rm $secrets
		mv $secrets.bak $secrets
	fi	

}

case "$1" in
	"enable")
		enable $2 $3;
		;;
	"disable")
		disable $2;
		;;
	"password")
		password $2 $3 $4;
		;;
	*)
		echo $USAGE >&2
		exit 1
		;;
esac
