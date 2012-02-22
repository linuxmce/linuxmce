#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

PrevVer="$2"

if [[ -n "$PrevVer" ]]; then
	/usr/pluto/bin/sqlcvs_db_fixer.sh
fi
