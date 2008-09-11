#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

#<-mkr_b_via_b->
exit
#<-mkr_b_via_e->

function getCurrentLayout {
	currentLayout=$(cat $XConfFile 2>/dev/null  | grep "^.*Option.*\"XkbLayout\"" | cut -f5 | sed 's/"//g')
	currentVariant=$(cat $XConfFile 2>/dev/null | grep "^.*Option.*\"XkbVariant\"" | cut -f5 | sed 's/"//g')

	if [[ "$currentLayout" == "" ]]; then
		currentLayout="us"
	fi
	if [[ "$currentVariant" == "" ]] ;then
		currentVariant="basic"
	fi

	echo $currentLayout $currentVariant
}

function setCurrentLayout {
	local layout=$1
	local variant=$2
	
	if [[ ! -f $XConfFile ]] ;then
		echo "Warning: file $XConfFile does not exist"
		return 1
	fi

	# Test is the Layout option is inable in X Config File
	grep -q "^.*Option.*\"XkbLayout\".*" $XConfFile
	if [[ $? -eq 0 ]]; then
		# Just replace the current layout
		sed "s|^.*Option.*\"XkbLayout\".*|\tOption\t\t\"XkbLayout\"\t\"$layout\"|g" $XConfFile > $XConfFile.$$
	else
		# Create the layout option
		sed "s|^.*Driver.*\"keyboard\"|\tDriver\t\t\"keyboard\"\n\tOption\t\t\"XkbLayout\"\t\"$layout\"|g" $XConfFile > $XConfFile.$$
	fi
	mv $XConfFile.$$ $XConfFile
	
	# Test if the Variant option is enabled in X Config File
	grep -q "^.*Option.*\"XkbVariant\".*" $XConfFile
	if [[ $? -eq -0 ]]; then
		# Just replace the current variant
		sed "s|^.*Option.*\"XkbVariant\".*|\tOption\t\t\"XkbVariant\"\t\"$variant\"|g" $XConfFile > $XConfFile.$$
	else
		# Create the layout option
		sed "s|^.*Driver.*\"keyboard\"|\tDriver\t\t\"keyboard\"\n\tOption\t\t\"XkbVariant\"\t\"$variant\"|g" $XConfFile > $XConfFile.$$
	fi
	mv $XConfFile.$$ $XConfFile
}

Action=$1
IP=$2
KbLayout=$3
KbVariant=$4
if [[ "$KbVariant" == "" ]] ;then
	KbVariant="basic"
fi


# Where is the X Config stored
if [[ "$IP" != "127.0.0.1" ]]; then 
	Q="SELECT PK_Device FROM Device WHERE IPaddress LIKE '$IP'"
	PK_Device=$(RunSQL "$Q")

	XConfPath="/usr/pluto/diskless/$PK_Device"
else
	XConfPath=""
fi

XConfFile="$XConfPath/etc/X11/xorg.conf"

case "$Action" in
	get)
		getCurrentLayout
	;;

	set)
		setCurrentLayout $KbLayout $KbVariant
		if [[ "$IP" != "127.0.0.1" ]]; then
			ssh $IP "DISPLAY=:0 setxkbmap $KbLayout $KbVariant" &>/dev/null
		else
			DISPLAY=:0 setxkbmap $KbLayout $KbVariant 2>/dev/null
		fi
	;;

	*)
		echo "Usage $0 <Action> <IP> [KbLayout]"
		exit 0 
	;;
esac
