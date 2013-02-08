#!/bin/bash
#=========================================================================|
#                                                                         |
# LinuxMCE fax processing script                                          |
#                                                                         |
# Filename: Fax_Update_Status.sh                                          |
# Type: 	BASH script                                                   |
# Author: 	Serge WAGENER (foxi352) 	                                  |
# Creation: April 2012 					                                  |
#                                                                         |
#=========================================================================|

# $1=PK_FAX
# $2=Status

. /usr/pluto/bin/SQL_Ops.sh

LOGFILE="/var/log/pluto/faxlog"  #Set to /dev/null to disable logging
DATETIME=$(date +"%d-%m-%Y %H:%M:%S")

echo "$DATETIME	$0 called with arguments: $*" >>$LOGFILE

UseDB asterisk;
Q="UPDATE fax_list SET Result='$2' WHERE PK_fax=$1"
R=$(RunSQL "$Q")
