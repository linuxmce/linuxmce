#!/bin/bash
# Create configuration files for
# * SIP Phones (sip_additional_phones.conf)
# * SCCP phones (sccp_additional_phones.conf)
# * IAX phones (iax_additional_phones.conf)
# * Users (voicemail.conf extensions.conf)
#
# Version 0.01
#pushd /etc/asterisk
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh

# LinuxMCE hardcoded ID's
DEVICEDATA_PhoneType=29
DEVICEDATA_PhoneNumber=31
DEVICEDATA_Secret=128
DEVICEDATA_ServerIp=260
DEVICEDATA_EmergencyNumbers=296
DEVICEDATA_EmergencyPhoneLine=297
DEVICEDATA_CallToken=298
DEVICE_TelecomPlugIn=11
DEVICECATEGORY_HARDPHONE=90
DEVICECATEGORY_SOFTPHONE=91

DIAL_ALL_PHONES=
ALL_TRUNKS=

# Database variables
DBUser=asteriskuser
DBPass=lmce
DB_SIP_Device_Table="sip_devices"
DB_IAX_Device_Table="iax_devices"
DB_SCCP_Device_Table="sccpdevice"
DB_SCCP_Buttons_Table="buttonconfig"
DB_SCCP_Line_Table="sccpline"
DB_Extensions_Table='extensions'
DB_Voicemail_Users_Table='voicemail_users'
DB_PhoneLines_Table='phonelines'
DB_astconfig_Table='ast_config'

#asterisk internal astbd variables
astdbfamily="AMPUSER"
	
# Context names in dialplan
Context_From_Lmce="from-lmce-custom"
Context_Ext_Local="ext-local"

CreateDialAllPhones()
{
	# create the dial statement to dial all phones
	if [[ -n "$DIAL_ALL_PHONES" ]]; then
		DIAL_ALL_PHONES="$DIAL_ALL_PHONES&Local/$PhoneNumber@trusted"
	else
		DIAL_ALL_PHONES="Local/$PhoneNumber@trusted"
	fi
}

WriteExtLocal()
{
	# adds extension to dialplan SQL query buffer.
	PHONESSQL="$PHONESSQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES \
	('$Context_Ext_Local','$PhoneNumber','1','Macro','exten-vm,novm,$PhoneNumber'), \
	('$Context_Ext_Local','$PhoneNumber','2','Hangup','');
	"
	echo "exten=>$PhoneNumber,hint,$PhoneProtocol/$PhoneNumber" >> /etc/asterisk/hints.conf
	
}

WriteSipPhone()
{
	# adds configuration of current SIP phone to SQL query buffer.
	PHONESSQL="$PHONESSQL INSERT INTO $DB_SIP_Device_Table (name,defaultuser,port,mailbox,secret,callerid,context)
	VALUES ('$PhoneNumber','$PhoneNumber','$Port','$PhoneNumber@device','$Secret','device <$PhoneNumber>','from-internal');"
}

WriteIAXPhone()
{
	# test for need of CallToken
	UseDB
	SQL="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$DeviceID' AND FK_DeviceData='$DEVICEDATA_CallToken'";
	res=$(RunSQL "$SQL")
	if [[ $(Field 1 "$res") -eq 1 ]]; then
		callToken="yes"
	else
		callToken="no"
	fi	
	# adds configuration of current IAX phone to SQL query buffer.
	PHONESSQL="$PHONESSQL INSERT INTO $DB_IAX_Device_Table (name,username,mailbox, secret, callerid, requirecalltoken)
	VALUES ('$PhoneNumber','$PhoneNumber','$PhoneNumber@device','$Secret','device <$PhoneNumber>', '$callToken');"
}

WriteSccpPhone()
{
	# remove : from MAC address
	local name=$(echo SEP$MacAddress|sed 's/://g'|tr '[:lower:]' '[:upper:]')
	# adds configuration of current SCCP phone to SQL query buffer.
	PHONESSQL="$PHONESSQL INSERT INTO $DB_SCCP_Device_Table (type,name,description)
	VALUES ('7970','$name','LinuxMCE ext $PhoneNumber');"
	
	# adds buttons of current SCCP phone to SQL query buffer.
	PHONESSQL="$PHONESSQL INSERT INTO $DB_SCCP_Buttons_Table (device,instance,type,name)
	VALUES ('$name','1','line','$PhoneNumber');"
	 
	# adds configuration of current SCCP line to SQL query buffer.
	PHONESSQL="$PHONESSQL INSERT INTO $DB_SCCP_Line_Table (id,name,label,description,cid_name,cid_num,context)
	VALUES ('$PhoneNumber','$PhoneNumber','$PhoneNumber','$PhoneNumber','pl_$DeviceID','$PhoneNumber','from-internal');"
}

GetMacAddress()
{
	local SQL
	local R
	local Row
	
	SQL="SELECT MACaddress From Device Where PK_Device = $DeviceID"
	R=$(RunSQL "$SQL")
	for Row in $R; do
		MacAddress=$(Field 1 "$Row")
	done
	echo $MacAddress
}

RunConfigureScript()
{
	# The ConfigurationScripts for the SIP phone needs to run, as they put the correct
	# secret and other information that might have been changed by the web admin into
	# the phone.
	local SQL
	local R
	local Row
	local ConfigureScript
	local MACaddress
	local IPaddress
	UseDB
	SQL="SELECT ConfigureScript, MACaddress, IPaddress FROM DeviceTemplate JOIN Device On PK_DeviceTemplate = FK_DeviceTemplate WHERE PK_Device = $DeviceID;"
	R=$(RunSQL "$SQL")
	ConfigureScript=
	for Row in $R; do
		ConfigureScript=$(Field 1 "$Row")
		MACaddress=$(Field 2 "$Row")
		IPaddress=$(Field 3 "$Row")
	done
	if [[  "$ConfigureScript" != "" ]]; then
		echo Executing Configuration Script $ConfigureScript
		/usr/pluto/bin/$ConfigureScript -d $DeviceID -i $IPaddress -m $MACaddress -n $PhoneNumber -c $IntIP
	fi
}

WorkThePhones()
{
	# recreate hints config files. This is a workaround until realtime hints do work
	rm -f /etc/asterisk/hints.conf
	touch /etc/asterisk/hints.conf

	# Get All phone devices
	UseDB asterisk;
	SQL="SELECT * FROM view_lmce_phones;"
	R=$(RunSQL "$SQL")

	# Recreate phone entries in LinuxMCE asterisk realtime DB
	PHONESSQL="USE asterisk; SET AUTOCOMMIT=0; START TRANSACTION;
	DELETE FROM $DB_SIP_Device_Table;
	ALTER TABLE $DB_SIP_Device_Table AUTO_INCREMENT=1;
	DELETE FROM $DB_IAX_Device_Table;
	ALTER TABLE $DB_IAX_Device_Table AUTO_INCREMENT=1;
	DELETE FROM $DB_SCCP_Device_Table;
	DELETE FROM $DB_SCCP_Line_Table;
	DELETE FROM $DB_SCCP_Buttons_Table;
	DELETE FROM $DB_Extensions_Table WHERE context='$Context_Ext_Local';
	ALTER TABLE $DB_Extensions_Table AUTO_INCREMENT=1;
	"

	for Row in $R; do
		DeviceID=$(Field 1 "$Row")
	   	PhoneNumber=$(Field 2 "$Row")
	    PhoneType=$(Field 3 "$Row")
	    ServerIp=$(Field 4 "$Row")
	    Secret=$(Field 5 "$Row")
		# If it's a new device it does not yet have a phonenumber
		if [[ ! -n $PhoneNumber ]]; then
			# get next free extension
			UseDB asterisk
			SQL="SELECT MAX(ext)+1 AS free_ext FROM view_lmce_phones;"
			PhoneNumber=$(RunSQL "$SQL")
			if [[ $PhoneNumber -eq 1 ]]; then
				PhoneNumber=200
			fi
			
			# assign next free phonenumber
			UseDB
			SQL="UPDATE Device_DeviceData SET IK_DeviceData='$PhoneNumber' WHERE FK_Device='$DeviceID' AND FK_DeviceData='$DEVICEDATA_PhoneNumber'";
			res=$(RunSQL "$SQL")
			
			# create new device secret
			Secret=$(echo `</dev/urandom tr -dc a-z0-9-_% | head -c16`)
			SQL="UPDATE Device_DeviceData SET IK_DeviceData='$Secret' WHERE FK_Device='$DeviceID' AND FK_DeviceData='$DEVICEDATA_Secret'"
			res=$(RunSQL "$SQL")
		fi
		
		case "$PhoneType" in
				"SIP5061")
	                    Port=5061
	                    PhoneProtocol=SIP
	                    ;;
	            "SIP5060"|"SIP")
	                    Port=5060
	                    PhoneProtocol=SIP
	                    ;;
	            "SCCP")
	                    PhoneProtocol=SCCP
	                    # The SCCP phones need the MAC address to function
	                    MacAddress=$(GetMacAddress)
	                    ;;
	            "IAX")
	            		PhoneProtocol=IAX
	                    ;;
	            "IAX2")
	            		PhoneProtocol=IAX2
	                    ;;
	    esac
		# Add phone to asterisk config files
		echo "Adding $PhoneProtocol phone device $DeviceID extension $PhoneNumber"
		# create device configuration entries
		case "$PhoneProtocol" in
			"SIP")
				RunConfigureScript
				WriteSipPhone
				if [[ reloadSIP -eq 0 ]]; then
					
					reloadSIP=1
				fi
				;;
			"SCCP")
				WriteSccpPhone
				;;
			"IAX"|"IAX2")
				WriteIAXPhone
				;;
			*)
				Logging "Phone" $SEVERITY_WARNING "Add All Phones" "Unknown Phone Protocol $PhoneProtocol";;
		esac
		# Create dialplan entries
		WriteExtLocal
		CreateDialAllPhones
	done
	PHONESSQL="$PHONESSQL COMMIT;"
	echo "Committing phones to DB";echo
	mysql -A -N -u$DBUser -p$DBPass asterisk -e "$PHONESSQL"
}

WriteVoiceMailEntries()
{
	# adds configuration of current voicemail entry to SQL query buffer.
	USERSSQL="$USERSSQL INSERT INTO $DB_Voicemail_Users_Table (mailbox,password,fullname,context,email) \
		VALUES ('$Extension','$Extension','$FirstName','default', '$ForwardEmail');"
}

WorkTheUsers()
{
	# Get devices
	UseDB
	SQL="Select Extension,FirstName,LastName,Nickname,HasMailbox,ForwardEmail From Users Order By PK_Users"
	R=$(RunSQL "$SQL")
	
	# Recreate phone entries in LinuxMCE asterisk realtime DB
	USERSSQL="USE asterisk; SET AUTOCOMMIT=0; START TRANSACTION;
	DELETE FROM $DB_Voicemail_Users_Table;
	ALTER TABLE $DB_Voicemail_Users_Table AUTO_INCREMENT=1;
	"
	for Row in $R; do
		Extension=$(Field 1 "$Row")
		FirstName=$(Field 2 "$Row")
	    HasMailbox=$(Field 5 "$Row")
	    ForwardEmail=$(Field 6 "$Row")
	    echo "Working on user $FirstName($Extension)"
		if [[ "$HasMailbox" == "1" ]]; then
		        WriteVoiceMailEntries
		fi
	done
	# adds configuration of Default voicemail entry to SQL query buffer.
	USERSSQL="$USERSSQL INSERT INTO $DB_Voicemail_Users_Table (mailbox,password,fullname,context,attach,envelope) \
		VALUES ('100','100','Default','default','no','no');"
		
	USERSSQL="$USERSSQL COMMIT;"
	echo "Committing users to DB";echo
	mysql -A -N -u$DBUser -p$DBPass asterisk -e "$USERSSQL"
}

AddTrunk()
{
	# we don't use old IAX protocol
	if [[ $protocol == "IAX" ]]; then protocol="IAX2"; fi
	
	# register with external provider and create peer
	case "$protocol" in
		"IAX2")
			# provider registry
			LINESSQL="$LINESSQL INSERT INTO $DB_astconfig_Table (var_metric,filename,category,var_name,var_val) VALUES
				('$(( 100+$id ))', 'iax.conf', 'general', 'register', '$username:$password@$host');"
			# TODO: create IAX2 peer
        ;;
        "SIP")
        	# provider registry
			LINESSQL="$LINESSQL INSERT INTO $DB_astconfig_Table (var_metric,filename,category,var_name,var_val) VALUES
				('$(( 100+$id ))', 'sip.conf', 'general', 'register', '$username:$password@$host/$phonenumber');"
			# create SIP peer
			context="from-trunk"
			LINESSQL="$LINESSQL INSERT INTO $DB_SIP_Device_Table (name,defaultuser,secret,host,port,context,qualify,nat,type,fromuser,fromdomain,callerid,allow,insecure) VALUES \
			('$host','$username','$password','$host','5060','$context','no','yes','friend',NULL,'$host','$username','alaw;ulaw','port,invite'), \
			('$phonenumber','$username','$password','$host','5060','$context','no','yes','peer','$username', '$host','$username','alaw;ulaw','port,invite');"
        ;;
    esac
	
	# add outbound context in realtime extensions
	context="outbound-allroutes"
	LINESSQL="$LINESSQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES \
	('$context','_$prefix.','1','Macro','dialout-trunk,$protocol/$phonenumber,\${EXTEN:1},,'),\
	('$context','_$prefix.','2','Macro','outisbusy,');"
	
	# create incoming context and redirect to realtime
	echo "
[from-trunk-$phonenumber]
switch => Realtime
	" >> /etc/asterisk/extensions-lmce-trunks.conf

	# add inbound context in realtime extensions
	context="from-trunk-$phonenumber"
	LINESSQL="$LINESSQL DELETE FROM $DB_Extensions_Table WHERE context like '$context';"
	LINESSQL="$LINESSQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES \
	('$context','_.','1','Set','GROUP()=OUT_$LineCounter'), \
	('$context','_.','2','Goto','from-trunk,\${EXTEN},1');"
	
	# add external did catch
	context="ext-did"
	line=$((100+$id))
	LINESSQL="$LINESSQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES \
	('$context','$username','1','Set','__FROM_DID=\${EXTEN}'), \
	('$context','$username','2','Set','CALLERID(num)=\${CALLERID(name)}'), \
	('$context','$username','3','Noop','Incoming call from \${CALLERID(num)}'), \
	('$context','$username','4','Set','FAX_RX='), \
	('$context','$username','5','Goto','custom-linuxmce,$line,1');"
}

WorkTheLines()
{
	# remove incoming contexts file
	rm -f /etc/asterisk/extensions-lmce-trunks.conf
	touch /etc/asterisk/extensions-lmce-trunks.conf

	# get emergency numbers
	UseDB
	SQL="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DEVICEDATA_EmergencyNumbers AND FK_Device=$DEVICE_TelecomPlugIn;"
	emergencynumbers=$(RunSQL "$SQL")
	
	# get default outgoing phoneline for emergency numbers
	SQL="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DEVICEDATA_EmergencyPhoneLine AND FK_Device=$DEVICE_TelecomPlugIn;"
	emergencyphonelineID=$(RunSQL "$SQL")

	# get phonenumber for outgoing phoneline ID
	UseDB asterisk
	SQL="SELECT protocol,phonenumber FROM $DB_PhoneLines_Table WHERE id=$emergencyphonelineID;"
	R=$(RunSQL "$SQL")
	emergencyphonelineProtocol=$(Field 1 "$R")
	emergencyphonelineNumber=$(Field 2 "$R")

	# remove everything before recreating	
	LINESSQL="SET AUTOCOMMIT=0; START TRANSACTION;"
	LINESSQL="$LINESSQL DELETE FROM $DB_astconfig_Table WHERE filename='sip.conf' AND category='general' AND var_name='register';"
	LINESSQL="$LINESSQL DELETE FROM $DB_astconfig_Table WHERE filename='iax.conf' AND category='general' AND var_name='register';"
	LINESSQL="$LINESSQL DELETE FROM $DB_Extensions_Table WHERE context='ext-did';"
	LINESSQL="$LINESSQL DELETE FROM $DB_Extensions_Table WHERE context='outbound-allroutes';"

	for number in ${emergencynumbers//,/ }; do
		LINESSQL="$LINESSQL INSERT INTO $DB_Extensions_Table (context,exten,priority,app,appdata) VALUES \
		('outbound-allroutes','$number','1','Macro','dialout-trunk,$emergencyphonelineProtocol/$emergencyphonelineNumber,\${EXTEN},,'), \
		('outbound-allroutes','$number','2','Macro','outisbusy,');"
		echo "Routing emergency number: $number via $emergencyphonelineProtocol/$emergencyphonelineNumber"
	done

	SQL="SELECT id,protocol,name,host,username,password,prefix,enabled,phonenumber FROM $DB_PhoneLines_Table"
	R=$(RunSQL "$SQL")
	for Row in $R; do
		id=$(Field 1 "$Row")
		protocol=$(Field 2 "$Row")
		name=$(Field 3 "$Row")
		host=$(Field 4 "$Row")
		username=$(Field 5 "$Row")
		password=$(Field 6 "$Row")
		prefix=$(Field 7 "$Row")
		enabled=$(Field 8 "$Row")
		phonenumber=$(Field 9 "$Row")
		echo "Working phoneline $id-$name ($protocol/$phonenumber)"
		if [[ $enabled == "yes" ]]; then AddTrunk; fi
	done
	# TODO: check if we need that in outbound context end: exten => foo,1,Noop(bar) ?

	LINESSQL="$LINESSQL COMMIT;"
	echo "Committing lines to DB";echo
	RunSQL "$LINESSQL"
}

GetTimeout() 
{
	SQL="SELECT IK_DeviceData FROM Device_DeviceData JOIN Device ON FK_Device = PK_Device WHERE FK_DeviceTemplate =34 and FK_DeviceData=247;"
	TIMEOUT=$(RunSQL "$SQL")
}

users2astdb()
{
	echo "Users2astdb: rebuilding $astdbfamily tree"
	asterisk -r -x "database deltree $astdbfamily" >> /dev/null
	
	UseDB
	
	S="SELECT PK_Device AS device, edd.IK_DeviceData AS extension, pdd.IK_DeviceData AS password
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		JOIN Device_DeviceData AS pdd ON PK_Device = pdd.FK_Device
		JOIN Device_DeviceData AS edd ON PK_Device = edd.FK_Device
		WHERE DeviceTemplate.FK_DeviceCategory IN ($DEVICECATEGORY_HARDPHONE,$DEVICECATEGORY_SOFTPHONE)
		AND pdd.FK_DeviceData=$DEVICEDATA_Secret AND edd.FK_DeviceData=$DEVICEDATA_PhoneNumber;"

	R=$(RunSQL "$S")

	for Row in $R; do
		name="pl_$(Field 1 "$Row")"
		extension=$(Field 2 "$Row")
		password=$(Field 3 "$Row")
		ringtimer=0
		noanswer=0
		recording="out=|in="
		
		#echo "Adding User $extension"
		asterisk -r -x "database put $astdbfamily $extension/password $password" >>/dev/null
		asterisk -r -x "database put $astdbfamily $extension/ringtimer $ringtimer" >>/dev/null
		asterisk -r -x "database put $astdbfamily $extension/noanswer $noanswer" >>/dev/null
		asterisk -r -x "database put $astdbfamily $extension/recording $recording" >>/dev/null
		asterisk -r -x "database put $astdbfamily $extension/outboundcid \"\\\"$name\\\" <$extension>\"" >>/dev/null
		asterisk -r -x "database put $astdbfamily $extension/cidname $name" >>/dev/null
	done	
}

devices2astdb()
{
	astdbdevice="DEVICE"
	echo "Devices2astdb: rebuilding $astdbdevice tree"
	asterisk -r -x "database deltree $astdbdevice" >> /dev/null
	
	UseDB
	S="SELECT (SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=Device.PK_Device AND FK_DeviceData=$DEVICEDATA_PhoneNumber) AS extension, 
		(SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=Device.PK_Device AND FK_DeviceData=$DEVICEDATA_PhoneType) AS type,
		(SELECT CONCAT('pl_',CAST(Device.PK_Device AS CHAR))) AS name
		FROM Device,DeviceTemplate 
		WHERE Device.FK_DeviceTemplate = DeviceTemplate.PK_DeviceTemplate AND DeviceTemplate.FK_DeviceCategory IN ($DEVICECATEGORY_HARDPHONE,$DEVICECATEGORY_SOFTPHONE);"
	
	S="SELECT PK_Device AS device, edd.IK_DeviceData AS extension, tdd.IK_DeviceData AS type
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		JOIN Device_DeviceData AS edd ON PK_Device = edd.FK_Device
		JOIN Device_DeviceData AS tdd ON PK_Device = tdd.FK_Device
		WHERE DeviceTemplate.FK_DeviceCategory
		IN ($DEVICECATEGORY_HARDPHONE,$DEVICECATEGORY_SOFTPHONE)
		AND edd.FK_DeviceData=$DEVICEDATA_PhoneNumber
		AND tdd.FK_DeviceData=$DEVICEDATA_PhoneType;"
	R=$(RunSQL "$S")
	
	for Row in $R; do
		#device=$(Field 1 "$Row")
		user=$(Field 2 "$Row")
		device=$user
		type="fixed"
		protocol=$(Field 3 "$Row")

		case "$protocol" in
			"SIP5061")
                protocol=SIP
            ;;
            "SIP5060")
                protocol=SIP
            ;;
        esac

		dial="$protocol/$user"
		
		#echo "Adding Device $device"
		asterisk -r -x "database put $astdbdevice $device/dial $dial" >>/dev/null
		asterisk -r -x "database put $astdbdevice $device/type $type" >>/dev/null
		asterisk -r -x "database put $astdbdevice $device/user $user" >>/dev/null
		
		if [[ "$user" != "none" ]]; then
			devices=$(asterisk -r -x "database get $astdbfamily $user/device")
			if [[ $devices =~ "not found" ]]; then
				devices=
			else
				devices="${devices#* }&"
			fi
			asterisk -r -x "database put $astdbfamily $user/device $devices$device" >>/dev/null
		fi
	done	

}

# Main worker flow

# fix permissions on each run
chmod g+w /etc/asterisk/*

GetTimeout

# work the different lmce devices
WorkThePhones
WorkTheUsers
WorkTheLines

# write values to internal astdb
users2astdb
devices2astdb

# fix some permisions
chown asterisk.asterisk /usr/share/asterisk/agi-bin/* /etc/asterisk/*

# reload needed asterisk modules to activate config
asterisk -r -x "sip reload"
asterisk -r -x "iax2 reload"
asterisk -r -x "sccp reload" >> /dev/null

#popd
