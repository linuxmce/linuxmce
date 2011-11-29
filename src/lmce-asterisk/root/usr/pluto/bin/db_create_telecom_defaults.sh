#!/bin/bash
#
# Create LinuxMCE telecom routing defaults for all users and lines
#
# Version 1.0 - 27. sep 2011 - Serge Wagener (foxi352) - first version
# Version 1.1 - 29. nov 2011 - Serge Wagener (foxi352) - added fax support

DB_Extensions_Table='extensions'
DB_Phonelines_Table='phonelines'
Context_From_Lmce="from-lmce-custom"
Context_Voice_Menu="voice-menu-lmce-custom"

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh

DB_telecom=pluto_telecom

GetUserExtensions()
{
	UseDB
	S="SELECT PK_Users,Extension from Users where Extension like '30_'"
	R=$(RunSQL "$S")
	for Row in $R; do
		USERS[$(Field 1 "$Row")]=$(Field 2 "$Row")
	done
	
	UseDB $DB_telecom
	S="SELECT DISTINCT EK_Users FROM UserRouting"
	R=$(RunSQL "$S")
	for Row in $R; do
		USERS[$(Field 1 "$Row")]="0"
	done
}

GetPhoneExtensions()
{
	UseDB
	S="SELECT FK_Device FROM Device_DeviceData WHERE FK_DeviceData=31"
	R=$(RunSQL "$S")
	for Row in $R; do
		PHONES="${PHONES}$(Field 1 "$Row"),"
	done
	PHONES="$(echo $PHONES|sed 's/\(.*\)./\1/')";
}

GetLineExtensions()
{
	UseDB asterisk
	S="SELECT id, protocol, isfax FROM $DB_Phonelines_Table"
	R=$(RunSQL "$S")
	for Row in $R; do
		if [[ $(Field 3 "$Row") == 'yes' ]]; then
			LINES[$(Field 1 "$Row")]='FAX'
		else
			LINES[$(Field 1 "$Row")]=$(Field 2 "$Row")
		fi
	done

	UseDB $DB_telecom
	S="SELECT DISTINCT ID FROM Line_HouseMode ORDER BY ID"
	R=$(RunSQL "$S")
	for Row in $R; do
		if [[ ${LINES[$(Field 1 "$Row")]} != 'FAX' ]]; then
			LINES[$(Field 1 "$Row")]="0"
		fi
	done
}

CreateUserRoutings()
{
	SQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	for key in "${!USERS[@]}"; do
		if [[ ${USERS[$key]} != "0" ]]; then
			echo "Add defaults for user $key"
			SQL="$SQL INSERT INTO UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) VALUES ('$key','1','0','1','ring,$PHONES');"
			SQL="$SQL INSERT INTO UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) VALUES ('$key','1','1','1','ring,$PHONES');"
		
			for ((i=2; i <= 4; i++)); do
				SQL="$SQL INSERT INTO UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) values ('$key','$i','0','1','voicemail,$key');";
				SQL="$SQL INSERT INTO UserRouting (EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing) values ('$key','$i','1','1','voicemail,$key');";
			done
		fi
	done
	
	SQL="$SQL COMMIT;"
	UseDB $DB_telecom
	R=$(RunSQL "$SQL")
}

CreateLineRoutings()
{
	SQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	for key in "${!LINES[@]}"; do
		if [[ ${LINES[$key]} != "0" ]]; then
			if [[ ${LINES[$key]} == "FAX" ]]; then
				echo "Add defaults for FAX line $key"
				SQL="$SQL DELETE FROM Line_HouseMode WHERE ID='$key'; ALTER TABLE Line_HouseMode AUTO_INCREMENT=1;"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','1','fax');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','2','fax');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','3','fax');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','4','fax');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','5','fax');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','6','fax');"
			else
				echo "Add defaults for line $key"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','1','ring,$PHONES');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','2','prompt');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','3','ring,$PHONES');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','4','prompt');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','5','ring,$PHONES');"
				SQL="$SQL INSERT INTO Line_HouseMode (ID,Type,EK_HouseMode,Routing) values ('$key','${LINES[$key]}','6','prompt');"
			fi
		fi
	done
	
	SQL="$SQL COMMIT;"

	UseDB $DB_telecom
	R=$(RunSQL "$SQL")
}

# fetch values from LinuxMCE
GetUserExtensions
GetPhoneExtensions
GetLineExtensions

CreateUserRoutings
CreateLineRoutings
