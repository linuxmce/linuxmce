#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

exit

if [[ "$OfflineMode" == "true" ]]; then
	echo "$0: OfflineMode is active, i can't share no IRCodes, skiping"
	exit 0
fi

# Getting the timestamp to use in description, which should be IR_YYYYMMDDHHmmss_PK_Installation
timestamp=`date +%Y%m%d%H%M%S`

# Check in dce,ir first since that may change the primary key in InfraredGroup_Command_Preferred
#/usr/pluto/bin/sqlCVS -H schema.linuxmce.org -R 3999 -C 3 $PLUTO_DB_CRED -D "$MySqlDBName" -a -n -r dce,ir -f /tmp/b1.dat -c "IR_${timestamp}_$PK_Installation" -U "share_ir_bot~nopass" -e checkin >>/var/log/pluto/Share_IR.log
#/usr/pluto/bin/sqlCVS -H schema.linuxmce.org -R 3999 -C 3 $PLUTO_DB_CRED -D "$MySqlDBName" -a -n -r local -f /tmp/b2.dat -t InfraredGroup_Command_Preferred -c "IR_${timestamp}_$PK_Installation" -U "share_ir_bot~nopass" -e checkin >>/var/log/pluto/Share_IR.log

