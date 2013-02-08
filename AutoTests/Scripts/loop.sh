#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

if [[ "$1" != "demonized" ]] ;then
        screen -S loop -d -m /bin/bash -x $0 demonized $1 >& /var/log/pluto/loop.log
        exit 0
fi

if [[ "$2" == "" ]] ;then
	Q="SELECT FK_Room FROM Device where PK_Device=$PK_Device"
	PK_Room=$(echo "$Q;" | /usr/bin/mysql --skip-column-names -h $MySqlHost pluto_main)
else
	PK_Room="$2"
fi

kill $(cat /var/loop.pid)
echo $$ > /var/loop.pid

#echo "---STARTING LOOP SCRIPT--" >> /var/log/pluto/pluto.log
#echo "---STARTING LOOP SCRIPT--" >> /var/log/pluto/DCERouter.log
#echo "---STARTING LOOP SCRIPT--" >> /var/log/pluto/22_Xine_Player.log
		

data=$(date)
echo "Starting loop.sh script at $data with PID $$" 1>>/loop.log



Q="select PK_File FROM File LEFT JOIN pluto_main.Device_DeviceData ON FK_Device=EK_Device AND FK_DeviceData=195 where EK_MediaType=5 AND IsDirectory=0 AND Missing=0 AND (IK_DeviceData=1 OR EK_Device IS NULL) AND Path not like '%samples%' limit 1";
VideoFile=$(echo "$Q;" | /usr/bin/mysql --skip-column-names -h $MySqlHost pluto_media)

Q="select PK_File FROM File LEFT JOIN pluto_main.Device_DeviceData ON FK_Device=EK_Device AND FK_DeviceData=195 where EK_MediaType=4 AND IsDirectory=0 AND Missing=0 AND (IK_DeviceData=1 OR EK_Device IS NULL) AND Path not like '%samples%' limit 1";
AudioFile=$(echo "$Q;" | /usr/bin/mysql --skip-column-names -h $MySqlHost pluto_media)

Q="select PK_File FROM File LEFT JOIN pluto_main.Device_DeviceData ON FK_Device=EK_Device AND FK_DeviceData=195 where EK_MediaType in (4,5) AND IsDirectory=0 AND Missing=0 AND (IK_DeviceData=1 OR EK_Device IS NULL) AND Path not like '%samples%'";

Files=$(echo "$Q;" | /usr/bin/mysql -h $MySqlHost pluto_media | tail +2 | tr '\n' ' ')

Q="select PK_Device from Device WHERE FK_Room=$PK_Room AND FK_DeviceTemplate=62"
PK_Device_Orbiter=$(echo "$Q;" | /usr/bin/mysql --skip-column-names -h $MySqlHost pluto_main)

Q="select PK_Device from Device WHERE FK_Room=$PK_Room AND FK_DeviceTemplate=5"
PK_Device_Xine=$(echo "$Q;" | /usr/bin/mysql --skip-column-names -h $MySqlHost pluto_main)

Q="select PK_Device from Device WHERE FK_DeviceTemplate=2"
PK_Device_Media_Plugin=$(echo "$Q;" | /usr/bin/mysql --skip-column-names -h $MySqlHost pluto_main)

Q="select FK_EntertainArea from Device_EntertainArea where FK_Device=$PK_Device_Orbiter"
PK_EntertainArea=$(echo "$Q;" | /usr/bin/mysql --skip-column-names -h $MySqlHost pluto_main)


while : ;do

echo "Video File $VideoFile" >>/loop.log
echo "Audio File $AudioFile" >>/loop.log
echo "All Files $Files" >>/loop.log

for File in $Files; do

	data=$(date)
	echo "($data) Starting tests for file $File" >>/loop.log
		
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 43 13 "!F$File" 45 $PK_EntertainArea
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Xine 1 41 43 4000
	sleep 2
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Xine 1 41 43 8000
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 741 159 212
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 741 159 223
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 741 159 224
	sleep 4
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Xine 1 41 43 32000
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 741 159 1
	sleep 3
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 741 159 224
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 741 159 1
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Xine 1 41 43 -32000
	sleep 3
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 741 159 224
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 43 13 "!F$VideoFile" 45 $PK_EntertainArea
	sleep 4

	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 44
	sleep 2
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 43 13 "!F$File" 45 $PK_EntertainArea
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 401 29 5
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 2
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 2
sleep 2
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 1
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 1
sleep 2
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 2
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 2
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 2
sleep 2
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 1
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 1
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 2
/usr/pluto/bin/MessageSend dcerouter 0 $PK_Device_Orbiter 1 9 30 2

	sleep 3
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 43 13 "!F$File" 45 $PK_EntertainArea
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 43 13 "!F$VideoFile" 45 $PK_EntertainArea
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 43 13 "!F$File" 45 $PK_EntertainArea
	sleep 3
	/usr/pluto/bin/MessageSend dcerouter $PK_Device_Orbiter $PK_Device_Media_Plugin 1 43 13 "!F$AudioFile" 45 $PK_EntertainArea
	sleep 3

	data=$(date)
        echo "($data) Tests for file $File done" >>/loop.log
done
done

echo 'Ending script loop.sh' 1>>/loop.log
