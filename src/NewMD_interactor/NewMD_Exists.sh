#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

MAC="$1"

Q="
	SELECT PK_Device
	FROM Device
	WHERE MACaddress='$MAC'
"
R=$(RunSQL "$Q")

[ -n "$R" ] && echo $R
