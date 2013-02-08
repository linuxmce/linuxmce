#
# bootmisc.sh	Miscellaneous things to be done during bootup.
#
# Version:	@(#)bootmisc.sh  2.85-17  04-Jun-2004  miquels@cistron.nl
#

DELAYLOGIN=yes
VERBOSE=yes
EDITMOTD=yes
[ -f /etc/default/rcS ] && . /etc/default/rcS

#
#	Put a nologin file in /etc to prevent people from logging in
#	before system startup is complete.
#
if [ "$DELAYLOGIN" = yes ]
then
	echo "System bootup in progress - please wait" > /etc/nologin
fi

#
#	Create /var/run/utmp so we can login.
#
: > /var/run/utmp
if grep -q ^utmp: /etc/group
then
	chmod 664 /var/run/utmp
	chgrp utmp /var/run/utmp
fi

#
#	Set pseudo-terminal access permissions.
#
if [ ! -e /dev/.devfsd ] && [ -c /dev/ttyp0 ]
then
	chmod -f 666 /dev/tty[p-za-e][0-9a-f]
	chown -f root:tty /dev/tty[p-za-e][0-9a-f]
fi

#
#	Update /etc/motd. If it's a symbolic link, do the actual work
#	in the directory the link points to.
#
if [ "$EDITMOTD" != no ]
then
	MOTD="`readlink -f /etc/motd || :`"
	if [ "$MOTD" != "" ]
	then
		uname -a > $MOTD.tmp
		sed 1d $MOTD >> $MOTD.tmp
		mv $MOTD.tmp $MOTD
	fi
fi

#
#	Save kernel messages in /var/log/dmesg
#
if [ -x /bin/dmesg ] || [ -x /sbin/dmesg ]
then
	dmesg -s 524288 > /var/log/dmesg
elif [ -c /dev/klog ]
then
	dd if=/dev/klog of=/var/log/dmesg &
	dmesg_pid=$!
	sleep 1
	kill $dmesg_pid
fi


#
#	Remove ".clean" files.
#
rm -f /tmp/.clean /var/run/.clean /var/lock/.clean

: exit 0

