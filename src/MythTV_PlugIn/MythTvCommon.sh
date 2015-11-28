#!/bin/sh
# /usr/pluto/bin/MythTvSyncDB.sh
# Copyright 2008 Daniel Kristjansson
# Licensed under the GPL, version 2; or, at your option, a later version.

BACKEND=/usr/bin/mythbackend
# If the user installed a their own mythbackend, adjust..
if test -f /usr/local/bin/mythbackend ; then
	BACKEND=/usr/local/bin/mythbackend
fi

FILLDB=/usr/bin/mythfilldatabase
FILLDB_LOG=/var/log/mythtv/mythfilldatabase.log
FILLDBINITRUN=/home/mythtv/MythTvInitialFillDBRun

# If the user installed a their own mythfilldatabase, adjust..
if test -f /usr/local/bin/mythfilldatabase ; then
	FILLDB=/usr/local/bin/mythfilldatabase
fi

# Run mythfilldatabase on a single core if possible
if test x"" != x"$(which taskset)" ; then
	FILLDB="taskset -c 0 "$FILLDB
fi

HAS_IGNORE_BACKEND=`$FILLDB --help 2>&1 | grep ignore-backend`
if test x"" != x"$HAS_IGNORE_BACKEND" ; then
	FILLDB=$FILLDB" --ignore-backend"
fi

DBHostName="localhost"
DBUserName="mythtv"
DBName="mythconverg"
DBPassword="mythtv"
DBPort="3306"

if [ -f /etc/mythtv/mysql.txt ]; then
	. /etc/mythtv/mysql.txt
fi

MYSQLPIPE="mysql --skip-column-names -B -h $DBHostName  -P $DBPort -u $DBUserName -p$DBPassword $DBName"
MYSQLPIPE_WITH_COL="mysql -B -h $DBHostName  -P $DBPort -u $DBUserName -p$DBPassword $DBName"
NAMES="empty"

UpdateMythSetting() {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"
	SETTING_HOST="$3"

	if [ x"$SETTING_HOST" = x"NULL" ] ; then
		echo "DELETE FROM settings WHERE value = '${SETTING_VALUE}' AND hostname IS NULL;" | $MYSQLPIPE
		echo "INSERT INTO settings(value,data) VALUES ('$SETTING_VALUE','$SETTING_DATA');" | $MYSQLPIPE
	else
		echo "DELETE FROM settings WHERE value = '${SETTING_VALUE}' AND hostname = '${SETTING_HOST}';" | $MYSQLPIPE
		echo "INSERT INTO settings(value,data,hostname) VALUES ('$SETTING_VALUE', '$SETTING_DATA', '$SETTING_HOST');" | $MYSQLPIPE
	fi
}

InsertMythSetting() {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"
	SETTING_HOST="$3"

	if [ x"$SETTING_HOST" = x"NULL" ] ; then
		COUNT=$(echo "SELECT count(1) FROM settings WHERE value = '${SETTING_VALUE}' AND hostname IS NULL;" | $MYSQLPIPE)
	else
		COUNT=$(echo "SELECT count(1) FROM settings WHERE value = '${SETTING_VALUE}' AND hostname = '${SETTING_HOST}';" | $MYSQLPIPE)
	fi

	if [ x"$COUNT" = x"0" ] ; then
		UpdateMythSetting "$SETTING_VALUE" "$SETTING_DATA" "$SETTING_HOST"
	fi
}

UpdateMythSettingAllHosts() {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"

	if [ x"$NAMES" = x"empty" ] ; then
		NAMES=$(echo "SELECT hostname FROM settings WHERE hostname IS NOT NULL GROUP BY hostname;" | $MYSQLPIPE)
	fi

	for NAME in $NAMES ; do
		UpdateMythSetting "$SETTING_VALUE" "$SETTING_DATA" "$NAME"
	done
}

InsertMythSettingAllHosts() {
	SETTING_VALUE="$1"
	SETTING_DATA="$2"

	if [ x"$NAMES" = x"empty" ] ; then
		NAMES=$(echo "SELECT hostname FROM settings WHERE hostname IS NOT NULL GROUP BY hostname;" | $MYSQLPIPE)
	fi

	for NAME in $NAMES ; do
		InsertMythSetting "$SETTING_VALUE" "$SETTING_DATA" "$NAME"
	done	
}

DownloadChannelIcons() {
	MASTER_ICON_MAP=/home/mythtv/master_iconmap.xml
	DESTINATION_DIR=/home/mythtv/channels

	# see if we can get a more up to date master icon map...
	TMP_MD5=`mktemp /tmp/master_icon_map.xml.md5sum.XXXXXX`
	TMPF=`mktemp /tmp/master_icon_map.xml.XXXXXX`
	wget --no-verbose --timeout=5 http://www.linuxmce.org/tvicons/master_iconmap.xml.md5sum -O $TMP_MD5
	wget --no-verbose --timeout=5 http://www.linuxmce.org/tvicons/master_iconmap.xml -O $TMPF
	DL_MD5SUM=$(cat $TMP_MD5 | awk '{ print $1 }')
	CA_MD5SUM=$(md5sum $TMPF |  awk '{ print $1 }')
	if [ x"$DL_MD5SUM" != x"" -a x"$DL_MD5SUM" = x"$CA_MD5SUM" ] ; then
		MASTER_ICON_MAP=$TMPF
	fi

	# actually download the icons
	$FILLDB --max-days 0 --import-icon-map $MASTER_ICON_MAP --update-icon-map

	rm -f $TMP $TMP_MD5

	# make sure destination directory exist and is in a usable state..
	mkdir -p $DESTINATION_DIR
	chown mythtv:mythtv $DESTINATION_DIR 2> /dev/null
	chmod a+rx  $DESTINATION_DIR 2> /dev/null
	chmod g+rwx $DESTINATION_DIR 2> /dev/null

	# Myth puts the icons in the current user's home directory. 
	# We want $DESTINATION_DIR and readable by all
	DIR=$(grep ^`whoami` /etc/passwd | awk '{ split($1, list, ":") ; print list[6] }')
	DIR=$DIR/.mythtv

	# Move the icon files
	if test x`ls $DIR/channels | wc -l` != x"0" ; then
		mv -f $DIR/channels/* $DESTINATION_DIR
	fi

	# Make sure they files are readable by all
	chmod -R a+r $DESTINATION_DIR 2> /dev/null

	# Tell MythTV that the channel icons have been moved.
	echo "UPDATE channel SET icon=REPLACE(icon,'$DIR','/home/mythtv')" | $MYSQLPIPE
}

SetMythTvInitialFillDBRun() {
	touch $FILLDBINITRUN ; chown mythtv:mythtv $FILLDBINITRUN
	date +%s > $FILLDBINITRUN
}

WaitUntilMythTvInitialRunXMinutesOld() {
	STARTTIME=`date +%s`
	if [ -r $FILLDBINITRUN -a -s $FILLDBINITRUN ] ; then
		STARTTIME=$(cat $FILLDBINITRUN)
	fi

	MINS="$1"
	if [ x"$MINS" = x"" ] ; then
		MINS=10
	fi
	SECS=$(echo $MINS \* 60 | bc)

	ENDTIME=$(echo $STARTTIME + $SECS | bc)

	NOW=`date +%s`
	if [ $ENDTIME -gt $NOW ] ; then
		TIMETOWAIT=$(echo $ENDTIME - $NOW | bc)
		if [ $TIMETOWAIT -gt 60 ] ; then
			echo Waiting $TIMETOWAIT seconds before starting..
			echo We do not want to start until it has been
			echo $MINS minutes since the MythTV channel download.
		fi
		while [ $ENDTIME -gt `date +%s` ] ; do
			sleep 1
		done

		# in case end time changed, redo if we waited more
		# than a few seconds
		TIMETOWAIT=$(echo $ENDTIME - $NOW | bc)
		if [ $TIMETOWAIT -gt 5 ] ; then
			WaitUntilMythTvInitialRunXMinutesOld $MINS
		fi
	fi

	echo WaitUntilMythTvInitialRunXMinutesOld done
}

