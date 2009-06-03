#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

URL="$1"
Table="$2"

if [[ -z "$URL" ]]; then
	echo "ERROR. Empty URL"
	exit 1
fi

OutputFile=$(mktemp)
trap "rm -f '$OutputFile'" EXIT
wget --timeout=10 -O "$OutputFile" "$URL"
# test if offline
RET=$?
if [[ "$RET" == "1" ]]; then
        /usr/pluto/bin/MessageSend "$DCERouter" 0 -305 1 741 159 53 9 'The device was added but is missing some config data which can be downloaded only from internet. The device will not work properly until that info is downloaded.'
        exit 1
fi

Header=$(head -1 "$OutputFile")
Footer=$(tail -1 "$OutputFile")

if [[ "$Header" != "-- Database import" || "$Footer" != "-- EOF" ]]; then
	echo "ERROR. Header: $Header; Footer: $Footer"
	exit 1
fi

PSC=$(head -2 "$OutputFile" | tail -1)
PSC="${PSC#-- psc_id: }"

if [[ "$Table" != ""  && "$PSC" != "" ]]; then
	Q="DELETE FROM $Table WHERE psc_id IN ($PSC) AND psc_mod=0"
	RunSQL "$Q"

	## Get a list of local modified records
	Q="SELECT psc_id FROM $Table WHERE psc_id IN ($PSC) AND psc_mod != 0"
	
	LocalModifiedPscIDs=$(RunSQL "$Q" | tr ' ' ',')
	if [[ "$LocalModifiedPscIDs" == ""  ]] ;then
		LocalModifiedPscIDs="0"
	fi
fi

mysql -f $MYSQL_DB_CRED "$MySqlDBName" <"$OutputFile"

if [[ "$Table" != ""  && "$PSC" != "" ]]; then
	## Reset the psc_mod of the records that where syncronized
	Q="UPDATE $Table SET psc_mod=0 WHERE psc_id IN ($PSC) AND psc_id NOT IN ($LocalModifiedPscIDs)"
	RunSQL "$Q"
fi

