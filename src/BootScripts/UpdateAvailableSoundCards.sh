#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/LockUtils.sh

WaitLock "UpdateAvailableSoundCards" "UASC"
SoundCards=$(/usr/pluto/bin/ListSoundCards.sh | tr '\n' ',')
SoundCards=${SoundCards%,}

DEVICEDATA_Available_Sound_Cards=287

Q="REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
	VALUES ('$PK_Device', '$DEVICEDATA_Available_Sound_Cards', '$SoundCards')"
RunSQL "$Q"
Unlock "UpdateAvailableSoundCards" "UASC"
