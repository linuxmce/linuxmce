#!/bin/bash
#=========================================================================|
#                                                                         |
# LinuxMCE fax processing script                                          |
#                                                                         |
# Filename: Fax_Send.sh                                                   |
# Type: 	BASH script                                                   |
# Author: 	Serge WAGENER (foxi352) 	                                  |
# Creation: April 2012 					                                  |
#                                                                         |
#=========================================================================|

# $1=file or mail
# $2=filename
# $3=destination number
# $4=phoneline
# $5=header
# $6=PK_fax of fax_list table in asterisk DB to update status

. /usr/pluto/bin/SQL_Ops.sh

LOGFILE="/var/log/pluto/faxlog"  #Set to /dev/null to disable logging
DATETIME=$(date +"%d-%m-%Y %H:%M:%S")

echo "$DATETIME	$0 called with arguments: $*" >>$LOGFILE

case $1 in
	"file")
		echo "$DATETIME	Faxing $2 to $3 via $4 (ID: $6)" >>$LOGFILE
		# Extract infile parts to details
		faxfile_extension=${2##*.}
		faxfile_name=$(basename "$2" .$faxfile_extension)
		faxfile_path=$(dirname "$2")
		
		# Create outfile path
		outfile="$faxfile_path/$faxfile_name.tiff"
		pdffile="$(date +"%Y-%m-%d %H.%M.%S") $faxfile_name.pdf"	
		pdffile="$faxfile_path/$(echo $pdffile | sed 's/ /_/g' | sed 's/:/./g' | sed 's/\//-/g')"
			
		# Convert input file to fax compatible .tiff
		gs -q -dNOPAUSE -dBATCH -sDEVICE=tiffg4 -sPAPERSIZE=a4 -sOutputFile="$outfile" "$2"
		rm -f "$2"
				
		# Modify filename in fax_list table with new pdf filename
		UseDB asterisk;
		Q="UPDATE fax_list SET FileName='$(basename $pdffile)' WHERE PK_fax=$6"
		R=$(RunSQL "$Q")

		echo "Channel:SIP/$4/$3
MaxRetries: 1
RetryTime: 300
WaitTime: 30
Context: fax-transmit
Extension: *71
Priority: 1
Set: EMAILADDR=
Set: STATIONID=$4
Set: HEADER=$5
Set: PK_FAX=$6
Set: FAXFILE=$outfile">/tmp/lmce_sendfax.call
		chown asterisk:asterisk /tmp/lmce_sendfax.call
		mv /tmp/lmce_sendfax.call /var/spool/asterisk/outgoing
		
		# Create PDF of faxed file for achive purposes
		tiff2pdf -f -p A4 -o "$pdffile" "$outfile"
		chown asterisk:asterisk $pdffile
		
		exit 0
		;;
	
	"mail")
		echo User:$UID > /tmp/uid
		cat |  sed 's/$//'>/tmp/email.eml
		exit 0
		;;
	*)
		exit 1
		;;
esac

