#!/bin/bash
#=========================================================================|
#                                                                         |
# LinuxMCE fax processing script                                          |
#                                                                         |
# Filename: Fax_Process.sh                                                |
# Type: 	BASH script                                                   |
# Author: 	Serge WAGENER (foxi352) 	                                  |
# Creation: April 2012 					                                  |
#                                                                         |
#=========================================================================|

# $1 = Filename without .tif
# $2 = Destination Email
# $3 = CID_NUMBER
# $4 = CID_NAME

. /usr/pluto/bin/SQL_Ops.sh

CoreID=1
DD_EmailSettings=304

LOGFILE="/var/log/pluto/faxlog"  #Set to /dev/null to disable logging
DATETIME=$(date +"%d-%m-%Y %H:%M:%S")
FORMAT="pdf"

FILE=$1
DEST_NUMBER=$2
CID_NUMBER=$3
CID_NAME=$4

# TODO: Replace by some real stuff
DEST_EXTEN="20408720"
USER_ID=1

SOURCEFILE=$FILE.tif
DESTFILE=$FILE.$FORMAT
INFOFILE=$FILE.txt

CommaField() {
	echo "$2" | cut -d, -f "$1"
}

# Fetch email configuration
Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$CoreID AND FK_DeviceData=$DD_EmailSettings"
R=$(RunSQL "$Q")
if [ -n "$R" ]; then
	FROM_NAME=$(CommaField 2 "$R")
	FROM_EMAIL=$(CommaField 3 "$R")
fi
if [ "$FROM_NAME" = "" ] ; then
  FROM_NAME="LinuxMCE"
fi

# Fetch user ID for fax forward
UseDB asterisk;
Q="SELECT faxmail FROM phonelines WHERE phonenumber=$DEST_EXTEN"
R=$(RunSQL "$Q")
USER_ID=$(Field 1 "$R")

# Fetch user's data for user with ID
UseDB;
Q="SELECT  FirstName,LastName,Nickname,ForwardEmail FROM Users WHERE PK_Users=$USER_ID"
R=$(RunSQL "$Q")
DEST_NAME="$(Field 1 "$R") $(Field 2 "$R")"
DEST_NICK=$(Field 3 "$R")
DEST_EMAIL=$(Field 4 "$R")

if [ "$FILE" = "" ]; then
  echo "Error: A file is required"
  echo "$DATETIME	ERROR: No file was given.">>$LOGFILE
  exit 1;
fi

if [ "$CID_NUMBER" = "" ] ; then
  CID_NUMBER="unknown"
fi

echo "$DATETIME	Called with arguments: $*" >>$LOGFILE
echo "$DATETIME	Processing fax file (without .tif extension) = $FILE">>$LOGFILE

RETVAL=1

if [ -e $SOURCEFILE ]; then
	# Read data from TIFF file
	PAGES=$(tiffinfo $SOURCEFILE | grep "Page" | cut -d "-" -f 2)
	DT=$(tiffinfo $SOURCEFILE | grep "Date")
	DTFAX=${DT#*: }
	COUNT=${PAGES#*-}
	if [ -z $COUNT ]; then
		# If didn't find a page count, use the number of occurrences of "spandsp"
		COUNT=$(grep -c "spandsp" $SOURCEFILE)
		if [ -z $COUNT ]; then
			COUNT="<unknown>"
		fi
	fi
    tiff2pdf -f -p letter $SOURCEFILE > $DESTFILE

	echo "" >$INFOFILE
	echo "Dear $DEST_NICK," >>$INFOFILE
	echo "" >>$INFOFILE
	echo "You have just received a $COUNT page fax:" >>$INFOFILE
	echo "" >>$INFOFILE
	echo "     From : $CID_NAME <$CID_NUMBER>" >>$INFOFILE
	echo "       To : $DEST_NAME, $DEST_EXTEN" >>$INFOFILE
	echo "     Date : $DTFAX" >>$INFOFILE
	echo "" >>$INFOFILE
	echo "The original fax document is attached in $FORMAT format." >>$INFOFILE
	
	mail -r "$FROM_NAME <$FROM_EMAIL>" -s "New fax from $CID_NAME <$CID_NUMBER>" -a $DESTFILE $DEST_EMAIL < $INFOFILE
	
	echo "$DATETIME	$COUNT page(s) fax received from $CID_NUMBER @ $DTFAX">>$LOGFILE
	echo "$DATETIME	Sent pdf converted fax as attachment from $FROM_NAME <$FROM_EMAIL> to $DEST_EMAIL">>$LOGFILE

	# Exit with OK status code
	RETVAL=0
fi


# Delete the temporary message file
rm -f $FILE.part1
rm -f $INFOFILE
rm -f $SOURCEFILE

# Archive faxes for now
mkdir -p /home/fax
chown asterisk:asterisk /home/fax
ARCHIVEFILE="$DTFAX $(basename $DESTFILE)"
ARCHIVEFILE="/home/fax/$(echo $ARCHIVEFILE | sed 's/ /_/g' | sed 's/\//-/g')"
mv -f $DESTFILE $ARCHIVEFILE
chown asterisk:asterisk $ARCHIVEFILE
chmod 666 $ARCHIVEFILE
echo "$DATETIME	Moved $DESTFILE to $ARCHIVEFILE">>$LOGFILE

echo "----------">>$LOGFILE
exit $RETVAL
