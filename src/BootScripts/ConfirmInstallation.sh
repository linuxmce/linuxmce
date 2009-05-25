#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

if [[ $OfflineMode == "true" ]]; then
	# We are in offline mode, we can't do that wget
	exit 0
fi

Q="SELECT COUNT(*) FROM Installation WHERE PK_Installation=1"
R=$(RunSQL "$Q")

if [[ "$R" -ne 0 ]]; then
	NewInstNumber=$(wget --timeout=10 -O - http://www.linuxmce.org/new_installation.php)
	if [[ -n "$NewInstNumber" ]]; then
	echo "Running command: /usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -D "$MySqlDBName" -t Installation -O 1 -N "$NewInstNumber" change_key"
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -D "$MySqlDBName" -t Installation -O 1 -N "$NewInstNumber" change_key
		ConfSet PK_Installation "$NewInstNumber"
		mv /usr/pluto/orbiter/floorplans/inst{1,$NewInstNumber}
	fi
fi

