#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

PHONE_CATEGORY="89"

SIP_DEVICE_DATA="29"
PHONENUM_DEVICE_DATA="31"

echo "Syncronizing Pluto Database with Asterisk..."

get_phone_categories() {
	if [ -z "$Category" ]; then
		Category=$PHONE_CATEGORY
		PhoneCategories=$PHONE_CATEGORY 
	fi
	Q="select PK_DeviceCategory from DeviceCategory where FK_DeviceCategory_Parent='$Category'"
	R=$(RunSQL "$Q")

	for DeviceCategory in $R; do
		PhoneCategories="${PhoneCategories}, $(Field 1 $DeviceCategory)"
		Category=$(Field 1 $DeviceCategory);
		get_phone_categories;
	done
	
}

#delete all phones from asterisk
UseDB asterisk
Q="delete from sip_buddies where type='friend'";
RunSQL "$Q"
UseDB $MySqlDBName

#get current installation
Q="select FK_Installation from Device where PK_Device=${PK_Device}";
R=$(RunSQL "$Q")

Installation="$(Field 1 $R)"

#get phone categories
get_phone_categories

#get device templates which are in phone categories

Q="select Device.PK_Device from Device join DeviceTemplate on  FK_DeviceTemplate = PK_DeviceTemplate where FK_DeviceCategory in (${PhoneCategories}) and FK_Installation=$Installation"
R=$(RunSQL "$Q")

#syncronize
for Device in $R; do	
	PK_Device="$(Field 1 $Device)";

	#check if SIP
	Q="select IK_DeviceData from Device_DeviceData where FK_Device=${PK_Device} and FK_DeviceData=${SIP_DEVICE_DATA}";
	D=$(RunSQL "$Q")
	if [ -n "$(Field 1 "$D")" ]; then
		#get phone num
	    Q="select IK_DeviceData from Device_DeviceData where FK_Device=${PK_Device} and FK_DeviceData=${PHONENUM_DEVICE_DATA}";
	    D=$(RunSQL "$Q")
	
		PhoneNumber="$(Field 1 "$D")";

       if [ -z "$InternalChannels" ]; then
           InternalChannels="SIP/$(Field 1 "$D")"
       else
           InternalChannels="${InternalChannels}&SIP/$(Field 1 "$D")"
	   fi
											   


		UseDB asterisk
		Q="insert into sip_buddies (name, accountcode, callerid, canreinvite, context, host, disallow, type) values ('${PhoneNumber}', '${PhoneNumber}', 'device${PhoneNumber} <${PhoneNumber}>', 'N', 'trusted', 'dynamic', 'h263', 'friend');"
		RunSQL "$Q"
		UseDB $MySqlDBName
	fi
done

# update internal lines for incoming cals
UseDB asterisk
Q="update extensions_table set appdata='INTERNALCHANNELS=${InternalChannels}' where id=27"
RunSQL "$Q"
UseDB $MySqlDBName

#echo "$PhoneCategories"

#reboot asterisk
#asterisk -rx "restart gracefully" || true

echo "Action: Login
Username: admin
Secret: adminsecret


Action: Command
Command: restart gracefully


" | tr '\n' '.' | sed 's/\./\r\n/g' | nc -t localhost 5038 > /dev/null || true

echo "End of Syncronization."

