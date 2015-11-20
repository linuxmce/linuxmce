#!/bin/bash

. /usr/pluto/bin/Utils.sh

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

## Add bookmark for mediatomb web interface
AddBookmark "http://dcerouter:49152/" "Mediatomb UPnP Server"

## Create the main mount directory
mkdir -p /mnt/device

## Add a cron entry that scans for Internal Storage Devices
cronEntry="*/10 * * * * root bash -c '/usr/pluto/bin/StorageDevices_Radar.sh &>/dev/null'"
if [[ ! -e /etc/cron.d/StorageDevicesRadar ]] ;then
	echo "$cronEntry" >>/etc/cron.d/StorageDevicesRadar
	service cron reload || :
fi

## Add a cron entry that scans existing file servers for new shares
## On the core ONLY - l3mce
ConfEval
if [[ "$PK_Device" -eq "1" ]]; then
	cronEntry="*/10 * * * * root bash -c '/usr/pluto/bin/StorageDevices_FileServerRadar.sh &>/dev/null'"
	if [[ ! -e /etc/cron.d/StorageDevicesFileServerRadar ]] ;then
	        echo "$cronEntry" >>/etc/cron.d/StorageDevicesFileServerRadar
	        service cron reload || :
	fi
fi

## Samba Share Helper 
if [[ "$(id -u sambahelper 2>/dev/null)" == "" ]] ;then
	useradd -c "Pluto Samba Share Helper" -d /tmp -s /bin/false sambahelper
fi

if [[ ! -f /usr/pluto/var/sambaCredentials.secret ]] ;then
	. /usr/pluto/bin/Utils.sh
	smbUser="sambahelper"
	smbPass=$(GeneratePassword)
	mkdir -p /usr/pluto/var
	echo "
username=$smbUser
password=$smbPass
" > /usr/pluto/var/sambaCredentials.secret
fi

chmod 600 /usr/pluto/var/sambaCredentials.secret

## Add the sambahelper user to smbpasswd
if [[ -r /usr/pluto/var/sambaCredentials.secret ]] ;then
	smbpass=$(cat /usr/pluto/var/sambaCredentials.secret | grep '^password' | cut -d '=' -f2)

	smbuser=$(cat /usr/pluto/var/sambaCredentials.secret | grep '^user' | cut -d '=' -f2)
	smbuserid=$(id -u $smbuser)

	# This is the correct way to do it but is not tested
	# echo -e "${smbpass}\n${smbpass}" | smbpasswd -a "${smbuser}" -s

	# This is not the correct way but it works(tm)
	if ! BlacklistConfFiles '/etc/samba/smbpasswd' ;then
		if [ ! -e '/etc/samba/smbpasswd.pbackup' ] ;then
			if [ -e '/etc/samba/smbpasswd' ]; then
				cp /etc/samba/smbpasswd /etc/samba/smbpasswd.pbackup || :
			fi
		fi
		smbpass=$(/usr/pluto/bin/smbpass.pl $smbpass)
		echo "$smbuser:$LinuxUserID:$smbpass:[U          ]:LCT-00000001:,,," >> /etc/samba/smbpasswd 
	fi
fi


