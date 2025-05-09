#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Network_Parameters.sh

#if ! grep "1:2345:respawn:/usr/pluto/bin/Report_DCERouter_Status.sh" /etc/inittab; then
#	# Replace original tty1 program with our status screen
#	awk '
#		/1:2345:respawn:/ { print "1:2345:respawn:/usr/pluto/bin/Report_DCERouter_Status.sh" }
#		{ print }
#	' /etc/inittab >/etc/inittab.new
#	mv -f /etc/inittab.new /etc/inittab
#fi

if [ "$ShowStatus" == "no" ]; then
	exec /sbin/getty 38400 tty1
fi

exec </dev/tty1 >/dev/tty1 2>/dev/tty1

FStart=/tmp/pluto_status_menu.start
FNow=/tmp/pluto_status_menu.current
touch "$FStart" "$FNow"

RouterStatus()
{
	echo "$(date -R) Checking Router status" >>/var/log/pluto/DCERouter_Status.log
	if nc -z 127.0.0.1 3450; then
		echo "[1;32mRunning and accepting connections.[0m"
		echo "$(date -R) Running and accepting connections" >>/var/log/pluto/DCERouter_Status.log
	else
		echo "[1;5;31mNot responding on socket. Probably dead.[0m"
		echo "$(date -R) Not responding on socket. Probably dead." >>/var/log/pluto/DCERouter_Status.log
	fi
}

Menu()
{
	clear
	touch "$FStart"
	echo "[1mLast updated[0m: $(date -R)"
	echo "[1mDCERouter Status[0m: $(RouterStatus)"
	echo "$(NetworkConfig)"
	echo
	echo "[1mMenu:[0m"
	echo "[1m1[0m. Tail DCERouter log"
	echo "[1m2[0m. Tail pluto log"
	echo "[1m3[0m. Tail system log (/var/log/syslog)"
	echo "[1mX[0m. Exit and disable this screen"
	echo
	echo "[1mUsage:[0m"
	echo "Press a menu key to run that option"
	echo "Press [1mAlt+F2[0m through [1mAlt+F6[0m if you want to log into the system"
}

Tail()
{
	clear
	echo "[1m$Tail[0m: "
	tail "$Tail"
	echo "[1mPress any key to go back to the menu[0m"
	read -n 1 -s
	unset Tail
}

TimeDiff()
{
	local Diff=$(($2-$1))
	if [ "$#" -eq 2 ]; then
		echo "$Diff"
		return 0
	fi

	[ $((Diff<$3)) -eq 1 ]
}

ConfirmExit()
{
	local Confirmation="Yes, I am sure"

	echo
	echo "[31mThis action will close this status screen and also disable it.[0m"
	echo "You will get a login prompt in its place."
	echo "Are you sure you want to do this?"
	echo "Type '[1;33m$Confirmation[0m' if you agree"
	echo "Type anything else to cancel"
	echo -n "Answer: "
	read Answer
	if [ "$Answer" == "$Confirmation" ]; then
		ConfSet ShowStatus no
		return 0
	fi
	return 1
}

NetworkConfig()
{
	if [ -z "$DHCPsetting" ]; then
		echo "[1mIP address[0m: $ExtIP"
	else
		echo "[1mExternal IP address[0m: $ExtIP [1mInternal IP address[0m: $IntIP"
	fi
}

Exit=""
while [ -z "$Exit" ]; do
	[ -n "$Tail" ] && Tail
	Menu
	while TimeDiff $(stat -c '%X' "$FStart") $(stat -c '%X' "$FNow") 60; do
		Key=""
		read -s -n 1 -t 1 Key
		touch "$FNow"
		if [ -n "$Key" ]; then
			case "$Key" in
				1) Tail=/var/log/pluto/DCERouter.log; break ;;
				2) Tail=/var/log/pluto/pluto.log; break ;;
				3) Tail=/var/log/syslog; break ;;
				X) ConfirmExit && Exit="true"; break ;;
			esac
		fi
	done
done

echo
echo "[1mNote[0m: You can always re-enable this in /etc/pluto.conf"
echo "	by setting 'ShowStatus' to 'yes'"
