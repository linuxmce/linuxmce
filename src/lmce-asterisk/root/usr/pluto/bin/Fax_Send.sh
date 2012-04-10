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

case $1 in
	"file")
		echo "Sending $2 to $3"
		# Extract infile parts to details
		faxfile_extension=${2##*.}
		faxfile_name=$(basename "$2" .$faxfile_extension)
		faxfile_path=$(dirname "$2")
		
		# Create outfile path
		outfile="$faxfile_path/$faxfile_name.tiff"
		echo "Outfile $faxfile_name with extension $faxfile_extension in path $faxfile_path"
		gs -q -dNOPAUSE -dBATCH -sDEVICE=tiffg4 -sPAPERSIZE=a4 -sOutputFile="$outfile" "$2"
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
Set: FAXFILE=$outfile">/tmp/lmce_sendfax.call
		chown asterisk:asterisk /tmp/lmce_sendfax.call
		mv /tmp/lmce_sendfax.call /var/spool/asterisk/outgoing
		;;
	
	"mail")
		echo User:$UID > /tmp/uid
		cat |  sed 's/$//'>/tmp/email.eml
		;;
	*)
		exit 1
		;;
esac

