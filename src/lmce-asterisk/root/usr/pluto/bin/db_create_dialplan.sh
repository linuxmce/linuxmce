#!/bin/bash
#
# Create LinuxMCE dialplan for all house- and usermodes
#
# Version 1.0 - 27. sep 2011 - Serge Wagener (foxi352) - first version
# Version 1.1 - 29. nov 2011 - Serge Wagener (foxi352) - added fax support

DB_Extensions_Table='extensions'
Context_From_Lmce="from-lmce-custom"
Context_Voice_Menu="voice-menu-lmce-custom"

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh

# LinuxMCE hardcoded ID's
DEVICEDATA_Language=26

GetTimeout()
{
	UseDB
	SQL="SELECT IK_DeviceData FROM Device_DeviceData JOIN Device ON FK_Device = PK_Device WHERE FK_DeviceTemplate =34 and FK_DeviceData=247;"
	TIMEOUT=$(RunSQL "$SQL")
}

GetLanguage() 
{
	SQL="SELECT IK_DeviceData FROM Device_DeviceData JOIN Device ON FK_Device = PK_Device WHERE FK_DeviceTemplate =57 and FK_DeviceData = $DEVICEDATA_Language;"
	LANGUAGE=$(RunSQL "$SQL")
	
	case "$LANGUAGE" in
		"2")
			LANGUAGE='fr';
		;;
		"3")
			LANGUAGE='de';
		;;
		*)
			LANGUAGE='en';		
		;;
	esac
}

GetUserExtensions()
{
	UseDB
	S="SELECT PK_Users,UserName,Extension,ForwardEmail,HasMailbox FROM Users WHERE Extension LIKE '30%'"
	R=$(RunSQL "$S")
	for Row in $R; do
		USERS[$(Field 1 "$Row")]=$(Field 3 "$Row")
		# If user has a mailbox
		if [[  "$(Field 5 "$Row")" -eq 1 ]]; then
			USERSMB[$(Field 1 "$Row")]=$(Field 3 "$Row")
		fi;
		MAILS[$(Field 1 "$Row")]=$(Field 4 "$Row")
		NAMES[$(Field 1 "$Row")]=$(Field 2 "$Row")
	done
}

GetPhoneExtensions()
{
	UseDB
	S="SELECT FK_Device, IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=31"
	R=$(RunSQL "$S")
	for Row in $R; do
		PHONES[$(Field 1 "$Row")]=$(Field 2 "$Row")
	done
}

CreateDialplanLines()
{
	UseDB "pluto_telecom"
	S="select ID,EK_HouseMode,Routing from Line_HouseMode order by ID,EK_HouseMode"
	R=$(RunSQL "$S")

	SQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES \
	('$Context_From_Lmce','100','1','Voicemail','100'), \
	('$Context_From_Lmce','100','2','Hangup','');"

	for Row in $R; do
		Line="10$(echo $(Field 1 "$Row") | sed -e "s/^.*\(.\)$/\1/")"
		HouseMode=$(Field 2 "$Row")
		Routing=$(Field 3 "$Row")
		#Todo need to make this variable a setting in the system.
        Block_Anonymous="no"

		if [[  "$OldLine" != "$Line"  ]]; then	
			if [[ "$Block_Anonymous" = "yes" ]]; then
                SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
                ('$Context_From_Lmce','$Line','1','GotoIf','\$[\"\${CALLERID\(NUM\)}\" = \"anonymous\" | \"\${CALLERID\(NUM\)}\" =  \"0000000000\"] ?7:2)'),
                ('$Context_From_Lmce','$Line','2','GotoIf','\$[\"\${BLACKLIST()}\" = \"1\"]?7:3'),
                ('$Context_From_Lmce','$Line','3','AGI','lmce-phonebook-lookup.agi'),
                ('$Context_From_Lmce','$Line','4','AGI','lmce-gethousemode.agi'),
                ('$Context_From_Lmce','$Line','5','Set','CHANNEL(language)=\'$LANGUAGE\''),
                ('$Context_From_Lmce','$Line','6','Goto','$Context_From_Lmce,$Line-hm\${HOUSEMODE},1'),
                ('$Context_From_Lmce','$Line','7','Hangup','');"
            elif [[ "$Block_Anonymous" = "no" ]]; then 
                SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
                ('$Context_From_Lmce','$Line','1','GotoIf','\$[\"\${BLACKLIST()}\" = \"1\"]?6:2'),
                ('$Context_From_Lmce','$Line','2','AGI','lmce-phonebook-lookup.agi'),
                ('$Context_From_Lmce','$Line','3','AGI','lmce-gethousemode.agi'),
                ('$Context_From_Lmce','$Line','4','Set','CHANNEL(language)=\'$LANGUAGE\''),
                ('$Context_From_Lmce','$Line','5','Goto','$Context_From_Lmce,$Line-hm\${HOUSEMODE},1'),
                ('$Context_From_Lmce','$Line','6','Hangup','');"
            else        
                SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
                ('$Context_From_Lmce','$Line','1','AGI','lmce-phonebook-lookup.agi'),
                ('$Context_From_Lmce','$Line','2','AGI','lmce-gethousemode.agi'),
                ('$Context_From_Lmce','$Line','3','Set','CHANNEL(language)=\'$LANGUAGE\''),
                ('$Context_From_Lmce','$Line','4','Goto','$Context_From_Lmce,$Line-hm\${HOUSEMODE},1'),
                ('$Context_From_Lmce','$Line','5','Hangup','');"
            fi
		fi

		App="NoOp"
		AppParam="Do Nothing"
		
		if [[ $Routing == ring* ]]; then
			Rest=$(echo $Routing|cut -d"," -f2-)
			Number=
			AppParam=
			while [ "$Number" != "$Rest" ]
			do
				Number=$(echo $Rest|cut -d"," -f1)
				Rest=$(echo $Rest|cut -d"," -f2-)
				AppParam="${AppParam}Local/${PHONES[$Number]}@trusted/n&"
			done
			App="Dial"
			AppParam="$(echo $AppParam|sed 's/\(.*\)./\1/'),$TIMEOUT";
		fi

		if [[ $Routing == user* ]]; then
			User=$(echo $Routing|cut -d"," -f2)
			App="Dial"
			AppParam="Local/${USERS[$User]}@trusted/n,$TIMEOUT"
		fi

		if [[ $Routing == transfer* ]]; then
			User=$(echo $Routing|cut -d"," -f2)
			App="Dial"
			AppParam="Local/$User@trusted/n,$TIMEOUT"
		fi

		if [[ $Routing == voicemail* ]]; then
			User=$(echo $Routing|cut -d"," -f2)
			App="Macro"
			AppParam="vm,${USERS[$User]},DIRECTDIAL"
		fi

		if [[ $Routing == "prompt" ]]; then
			App="Goto"
			AppParam="voice-menu-lmce-custom,s,1"
		fi
		
		if [[ $Routing == fax ]]; then
			App="Goto"
			AppParam="fax,*70,1"
			SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
				('$Context_From_Lmce','$Line-hm$HouseMode','1','$App','$AppParam'),
				('$Context_From_Lmce','$Line-hm$HouseMode','2','Goto','$Context_From_Lmce,$Line-hm$HouseMode-\${DIALSTATUS},1'),
				('$Context_From_Lmce','$Line-hm$HouseMode','3','Hangup',''),
				('$Context_From_Lmce','$Line-hm$HouseMode-BUSY','1','Hangup',''),
				('$Context_From_Lmce','$Line-hm$HouseMode-NOANSWER','1','Hangup',''),
				('$Context_From_Lmce','$Line-hm$HouseMode-CONGESTION','1','Hangup',''),
				('$Context_From_Lmce','$Line-hm$HouseMode-CHANUNAVAIL','1','Hangup',''),
				('$Context_From_Lmce','$Line-hm$HouseMode-INVALIDARGS','1','Hangup','');"
		else
			SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
				('$Context_From_Lmce','$Line-hm$HouseMode','1','$App','$AppParam'),
				('$Context_From_Lmce','$Line-hm$HouseMode','2','Goto','$Context_From_Lmce,$Line-hm$HouseMode-\${DIALSTATUS},1'),
				('$Context_From_Lmce','$Line-hm$HouseMode','3','Hangup',''),
				('$Context_From_Lmce','$Line-hm$HouseMode-BUSY','1','Goto','$Context_From_Lmce,100,1'),
				('$Context_From_Lmce','$Line-hm$HouseMode-NOANSWER','1','Goto','voice-menu-lmce-custom,s,1'),
				('$Context_From_Lmce','$Line-hm$HouseMode-CONGESTION','1','Goto','$Context_From_Lmce,100,1'),
				('$Context_From_Lmce','$Line-hm$HouseMode-CHANUNAVAIL','1','Goto','$Context_From_Lmce,100,1'),
				('$Context_From_Lmce','$Line-hm$HouseMode-INVALIDARGS','1','Goto','$Context_From_Lmce,100,1');"
		fi
		OldLine=$Line
	done

	SQL="$SQL COMMIT;"

	UseDB "asterisk"
	R=$(RunSQL "$SQL")
}

CreateDialplanUsers()
{
	UseDB "pluto_telecom"
	S="SELECT EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing FROM UserRouting ORDER BY EK_Users,IsPriorityCaller,EK_UserMode,StepOrder"
	R=$(RunSQL "$S")

	SQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	Temp=
	for Row in $R; do
		if [[ -n ${USERS[$(Field 1 "$Row")]} ]]; then
			User=${USERS[$(Field 1 "$Row")]}
			UserMode=$(Field 2 "$Row")
			Pri=$(Field 3 "$Row")
			Try=$(Field 4 "$Row")
			Routing=$(Field 5 "$Row")
			
			#has to replaced by perl unless statement
			if [[  $Temp != $User*  ]]; then	
				SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
				('$Context_From_Lmce','$User','1','AGI','lmce-getusermode.agi'),
				('$Context_From_Lmce','$User','2','Goto','$Context_From_Lmce,$User-um\${USERMODE}-pri\${PRIORITYCALLER},1'),
				('$Context_From_Lmce','$User','3','Hangup','');"
			fi

			App="NoOp"
			AppParam="Do Nothing"
			if [[ $Routing == "ring," ]]; then
				AppParam=
				for i in "${PHONES[@]}"; do
					AppParam="${AppParam}Local/$i@trusted/n&"
				done
				App="Dial"
				AppParam="$(echo $AppParam|sed 's/\(.*\)./\1/'),$TIMEOUT";
			else
				if [[ $Routing == ring* ]]; then
					Rest=$(echo $Routing|cut -d"," -f2-)
					Number=
					AppParam=
					while [ "$Number" != "$Rest" ]
					do
						Number=$(echo $Rest|cut -d"," -f1)
						Rest=$(echo $Rest|cut -d"," -f2-)
						AppParam="${AppParam}Local/${PHONES[$Number]}@trusted/n&"
					done
					App="Dial"
					AppParam="$(echo $AppParam|sed 's/\(.*\)./\1/'),$TIMEOUT";
				fi
			fi
			
			if [[ $Routing == user* ]]; then
				i=$(echo $Routing|cut -d"," -f2)
				App="Dial"
				AppParam="Local/${USERS[$i]}@trusted/n,$TIMEOUT"
			fi

			if [[ $Routing == transfer* ]]; then
				i=$(echo $Routing|cut -d"," -f2)
				App="Dial"
				AppParam="Local/$i@trusted/n,$TIMEOUT"
			fi

			if [[ $Routing == voicemail* ]]; then
				i=$(echo $Routing|cut -d"," -f2)
				App="Macro"
				if [[ -n ${USER[$User]} ]]; then
					AppParam="vm,${USERS[$i]},DIRECTDIAL"
				else
					AppParam="vm,$User,DIRECTDIAL"
				fi
			fi

			if [[ $Temp -ne "$User-$Pri-$UserMode" ]]; then
				SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES 
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri','1','Goto','$Context_From_Lmce,$User-um$UserMode-pri$Pri-try$Try,1');"
			fi
			
			SQL="$SQL DELETE FROM $DB_Extensions_Table WHERE context = '$Context_From_Lmce' AND exten = '$User-um$UserMode-pri$Pri-try$Try' AND priority='1';
				DELETE FROM $DB_Extensions_Table WHERE context = '$Context_From_Lmce' AND exten = '$User-um$UserMode-pri$Pri-try$Try' AND priority='2';
				INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$Try','1','$App','$AppParam'),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$Try','2','Goto','$Context_From_Lmce,$User-um$UserMode-pri$Pri-try$Try-\${DIALSTATUS},1'),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$Try','3','Hangup',''),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$Try-BUSY','1','Goto','$Context_From_Lmce,$User-um$UserMode-pri$Pri-try$((Try+1)),1'),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$Try-NOANSWER','1','Goto','$Context_From_Lmce,$User-um$UserMode-pri$Pri-try$((Try+1)),1'),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$Try-CONGESTION','1','Goto','$Context_From_Lmce,$User-um$UserMode-pri$Pri-try$((Try+1)),1'),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$Try-CHANUNAVAIL','1','Goto','$Context_From_Lmce,$User-um$UserMode-pri$Pri-try$((Try+1)),1'),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$((Try+1))','1','Macro','vm,$User,DIRECTDIAL'),
				('$Context_From_Lmce','$User-um$UserMode-pri$Pri-try$((Try+1))','2','Hangup','');"
			Temp="$User-$Pri-$UserMode"
		fi
	done

	SQL="$SQL COMMIT;"
	
	UseDB "asterisk"
	R=$(RunSQL "$SQL")
}

CreateConferenceRooms()
{
	SQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
	('$Context_From_Lmce','_000.','1','ConfBridge','\${EXTEN},q'),
	('$Context_From_Lmce','_000.','2','Hangup','');"
	SQL="$SQL COMMIT;"
	UseDB "asterisk"
	R=$(RunSQL "$SQL")
}

CreateVoiceMenu()
{
	SQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	
	for i in "${PHONES[@]}"; do
		PhonesList="${PhonesList}Local/$i@trusted/n&"
	done

	PhonesList="$(echo $PhonesList|sed 's/\(.*\)./\1/')";
	SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
	('$Context_Voice_Menu','s','1','Answer',''),
	('$Context_Voice_Menu','s','2','Wait','1'),
	('$Context_Voice_Menu','s','3','AGI','lmce-callersforme.agi'),
	('$Context_Voice_Menu','s','4','Background','pluto/pluto-default-voicemenu'),
	('$Context_Voice_Menu','s','5','Set','TIMEOUT(digit)=10'),
	('$Context_Voice_Menu','s','6','Set','TIMEOUT(response)=20'),
	('$Context_Voice_Menu','t','1','Goto','$Context_Voice_Menu,s,1'),
	('$Context_Voice_Menu','0','1','Dial','$PhonesList,$TIMEOUT'),
	('$Context_Voice_Menu','0','2','Goto','$Context_Voice_Menu,s,1');"

	for i in "${!USERS[@]}"; do
		SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
		('$Context_Voice_Menu','$i','1','Goto','$Context_From_Lmce,${USERS[$i]},1'),
		('$Context_Voice_Menu','$i','2','Goto','s,1');"
	done

	SQL="$SQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES
	('$Context_Voice_Menu','#','1','VoiceMail','100'),
	('$Context_Voice_Menu','#','2','Hangup',''),
	('$Context_Voice_Menu','_XXX','1','Dial','Local/\${EXTEN}@trusted/n'),
	('$Context_Voice_Menu','_XXX','2','Hangup',''),
	('$Context_Voice_Menu','i','1','Background','pluto/invalid-entry'),
	('$Context_Voice_Menu','i','2','Goto','$Context_Voice_Menu,s,1');"

	SQL="$SQL COMMIT;"

	UseDB "asterisk"
	R=$(RunSQL "$SQL")
}

DeleteExistingDialplan ()
{
	SQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	#delete lines and users
	SQL="$SQL DELETE FROM $DB_Extensions_Table WHERE context='$Context_From_Lmce';"
	#delete voice menu
	SQL="$SQL DELETE FROM $DB_Extensions_Table WHERE context='$Context_Voice_Menu';"
	SQL="$SQL ALTER TABLE $DB_Extensions_Table AUTO_INCREMENT=1;"
	SQL="$SQL COMMIT;"

	UseDB "asterisk"
	R=$(RunSQL "$SQL")
}

# create LinuxMCE telecom routing defaults
/usr/pluto/bin/db_create_telecom_defaults.sh

# fetch values from LinuxMCE
GetTimeout
GetLanguage
GetUserExtensions
GetPhoneExtensions

# delete existing dialplan
DeleteExistingDialplan

#create new dialplan
CreateDialplanLines
CreateDialplanUsers
CreateConferenceRooms
CreateVoiceMenu

asterisk -r -x "core reload" >> /dev/null
#asterisk -r -x "dialplan reload" >> /dev/null
#asterisk -r -x "sip reload" >> /dev/null
