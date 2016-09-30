#!/bin/bash

Description="$1"
Number="$2"
Phone="$3"
RoomID="$4"
RoomName="$5"

if [[ "$Phone" == 0 ]]; then
	Phone=NULL
fi

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

ARRAYID_CommunicationScenarios=4
TEMPLATE_TelecomScenarios=17
COMMAND_MakeCall=921
COMMAND_PhoneExtension=83
COMMAND_ParamPK_DeviceTo=263
DEVICETEMPLATE_Telecom_Plugin=34

Q="
	SELECT PK_Device
	FROM Device
	WHERE FK_DeviceTemplate='$DEVICETEMPLATE_Telecom_Plugin'
"
TelecomPluginID=$(RunSQL "$Q")

Q="
	INSERT INTO CommandGroup (FK_Array, FK_Installation, Description,FK_Template,Hint)
	VALUES ($ARRAYID_CommunicationScenarios, $PK_Installation, '$Description', $TEMPLATE_TelecomScenarios, '$RoomName');
	SELECT LAST_INSERT_ID()
"
CommandGroupID=$(RunSQL "$Q")

Q="
	INSERT INTO CommandGroup_Room (FK_Room, FK_CommandGroup,Sort)
	VALUES ('$RoomID', '$CommandGroupID', '$CommandGroupID')
"
RunSQL "$Q"

Q="
	INSERT INTO CommandGroup_Command (FK_CommandGroup, FK_Command, FK_Device)
	VALUES('$CommandGroupID', '$COMMAND_MakeCall', '$TelecomPluginID');
	SELECT LAST_INSERT_ID()
"
CommandGroup_CommandID=$(RunSQL "$Q")

Q="
	INSERT INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command,FK_CommandParameter,IK_CommandParameter)
	VALUES ('$CommandGroup_CommandID', '$COMMAND_PhoneExtension', '$Number')
"
RunSQL "$Q"

Q="
	SELECT FK_CommandParameter
	FROM Command_CommandParameter
	WHERE FK_Command='$COMMAND_MakeCall'
"
CommandParms=$(RunSQL "$Q")

eval "CommandParm_$COMMAND_PhoneExtension='$Number'"
eval "CommandParm_$COMMAND_ParamPK_DeviceTo='$Phone'"

for Parm in $CommandParms; do
	FK_CommandParameter=$(Field 1 "$Parm")
	ParmVar="CommandParm_$FK_CommandParameter"
	ParmValue="${!ParmVar}"
	if [[ "$ParmValue" != NULL ]]; then
		ParmValue="'$ParmValue'"
	fi
	Q="
		INSERT IGNORE INTO CommandGroup_Command_CommandParameter (FK_CommandGroup_Command, FK_CommandParameter, IK_CommandParameter)
		VALUES ('$CommandGroup_CommandID', '$FK_CommandParameter', $ParmValue)
	"
	RunSQL "$Q"
done

DEVICECATEGORY_Mobile_Orbiter=2
DEVICECATEGORY_Standard_Orbiter=3
DEVICECATEGORY_Orbiter=5

Q="
	SELECT PK_Device
	FROM Device
	JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
	WHERE FK_DeviceCategory IN ($DEVICECATEGORY_Mobile_Orbiter, $DEVICECATEGORY_Standard_Orbiter, $DEVICECATEGORY_Orbiter)
"
OrbiterIDs=$(RunSQL "$Q")
OrbiterIDs="${OrbiterIDs// /,}"

Q="
	UPDATE Device
	SET NeedConfigure=1
	WHERE PK_Device IN ($OrbiterIDs)
"
RunSQL "$Q"

Q="
	UPDATE Orbiter
	SET Regen=1, Modification_LastGen='1970-01-01 00:00:00'
	WHERE PK_Orbiter IN ($OrbiterIDs)
"
RunSQL "$Q"
