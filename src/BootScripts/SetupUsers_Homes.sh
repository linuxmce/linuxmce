#!/bin/bash
# set system password files

echo "$(date -R) SetupUsers_Homes" >> /var/log/pluto/SetupUsers_Homes.log

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh

TemplateDir=/usr/pluto/templates

BaseDir="/home"
PublicHomeDir="/home/public/data"
MakeUsers=yes
while [[ "$#" -gt 0 ]]; do
	case "$1" in
		-e) ExtraDir="$2"; ExtraLink="$3"; shift; shift ;;
		-b) BaseDir="$2"; shift ;;
		-n) MakeUsers="no" ;;
	esac
	shift
done

if [ ! -e /etc/samba/smbpasswd.pbackup ] ;then
	cp /etc/samba/smbpasswd /etc/samba/smbpasswd.pbackup
fi

if [ ! -e /etc/samba/usermap.txt.pbackup ] ;then
	cp /etc/samba/usermap.txt /etc/samba/usermap.txt.pbackup 
fi

if [ ! -e /etc/group.pbackup ] ;then
	cp /etc/group /etc/group.pbackup 
fi

if [ ! -e /etc/passwd.pbackup ] ;then
	cp /etc/passwd /etc/passwd.pbackup 
fi

if [ ! -e /etc/shadow.pbackup ] ;then
	cp /etc/shadow /etc/shadow.pbackup 
fi

if [[ "$MakeUsers" == yes ]]; then
	# Extract non-pluto entries
	awk '!/^pluto_/' /etc/group >/etc/group.$$
	awk '!/^pluto_/' /etc/passwd >/etc/passwd.$$
	awk '!/^pluto_/' /etc/shadow >/etc/shadow.$$

	if ! BlacklistConfFiles '/etc/samba/smbpasswd' ;then
		cp $TemplateDir/smbpasswd.tmpl /etc/samba/smbpasswd.$$
	fi 
	: >/etc/samba/usermap.txt
fi

# get a list of all users
Q="SELECT PK_Users,UserName,Password_Unix,Password_Samba FROM Users"
R=$(RunSQL "$Q")
ListOfUsers="$R"
LinuxUserID=10000
DefaultSambaPassword=609FCABC7B0F9AEAAAD3B435B51404EE:DDFF3B733E17BE6500375694FE258864
DefaultLinuxPassword=

global_static_dirs="public temp_pvr mydvd cameras tv_listing"
user_dirs="audio pictures documents videos"
user_static_dirs="data"

UserList=
SambaUserShares=

for Users in $R; do
	PlutoUserID=$(Field 1 "$Users")
	UserName=$(Field 2 "$Users" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
	if [[ -z "$UserName" ]]; then
		continue
	fi
	LinuxPassword=$(Field 3 "$Users")
	SambaPassword=$(Field 4 "$Users")

	if [[ ! -d "$BaseDir/user_$PlutoUserID" ]]; then
		rm -rf "$BaseDir/user_$PlutoUserID"
	fi
	
	[[ -z "$LinuxPassword" ]] && LinuxPassword="$DefaultLinuxPassword"
	[[ -z "$SambaPassword" ]] && SambaPassword="$DefaultSambaPassword"
#	echo "$Users : $PlutoUserID - $UserName - $LinuxPassword - $SambaPassword"
	
	if [[ "$MakeUsers" == yes ]]; then
		SambaUserShares="${SambaUserShares}

		[$UserName]
		comment = $UserName's private files
		browseable = yes
		writable = yes
		create mask = 0770
		directory mask = 0770
		path = /home/user_$PlutoUserID
		public = no
		"
		# TODO: replace 00000001 with a real number of seconds (never set to 00000000)
		SambaEntry="pluto_$UserName:$LinuxUserID:$SambaPassword:[U          ]:LCT-00000001:,,,"
		ShadowEntry="pluto_$UserName:$LinuxPassword:12221:0:99999:7:::"
		PasswdEntry="pluto_$UserName:x:$LinuxUserID:$LinuxUserID:,,,:/home/user_$PlutoUserID:/bin/false"
		GroupEntry="pluto_$UserName:x:$LinuxUserID:www-data"
		SambaUnixMap="pluto_$UserName = $UserName"
		echo "$SambaEntry" >>/etc/samba/smbpasswd.$$
		echo "$ShadowEntry" >>/etc/shadow.$$
		echo "$PasswdEntry" >>/etc/passwd.$$
		echo "$GroupEntry" >>/etc/group.$$
		if ! BlacklistConfFiles '/etc/samba/usermap.txt' ;then
			echo "$SambaUnixMap" >>/etc/samba/usermap.txt
		fi
	fi

	mkdir -p -m 0770 "$BaseDir/user_$PlutoUserID"

	for dir in $user_static_dirs; do
		mkdir -p -m 0770 "$BaseDir/user_$PlutoUserID/${dir/~/ }"
	done
	
	for dir in $user_dirs; do

		mkdir -p -m 0770 "$BaseDir/user_$PlutoUserID/data/${dir/~/ }"
		if [[ -n "$ExtraDir" ]]; then
			Target="$ExtraDir/user_$PlutoUserID/data/${dir/~/ }/$ExtraLink"
			rm -f "$Target"
			ln -sf "$BaseDir/user_$PlutoUserID/data/${dir/~/ }" "$Target"
		fi
	done

	#rm -f "$BaseDir/$UserName"
	#ln -sf "user_$PlutoUserID" "$BaseDir/$UserName"
	UserList="$UserList $UserName"
# Files not updated at this point (woking on copies)
#	chown -R $UserName.$UserName /home/user_$PlutoUserID

	((LinuxUserID++))
done

if [[ "$MakeUsers" == yes ]] ;then
	PopulateSection "/etc/samba/smb.conf" "User Shares" "$SambaUserShares"
fi

for dir in $global_static_dirs; do
	mkdir -p -m 0755 "$BaseDir/${dir/~/ }"
	chmod 0755 "$BaseDir/${Dir/~/ }"
done

for dir in $user_static_dirs; do
	mkdir -p -m 0755 "$BaseDir/public/${dir/~/ }"
	chmod 0755 "$BaseDir/public/${dir/~/ }"
done

for dir in $user_dirs; do
	mkdir -p -m 0755 "$BaseDir/public/data/${dir/~/ }"
	chmod 0755 "$BaseDir/public/data/${dir/~/ }"
	if [[ -n "$ExtraDir" ]]; then
		Target="$ExtraDir/public/data/${dir/~/ }/$ExtraLink"
		rm -f "$Target"
		ln -sf "$BaseDir/public/data/${dir/~/ }" "$Target"
	fi
done

# Get a list of all Media Directors
Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=28"
R=$(RunSQL "$Q")

for Device in $R; do
	mkdir -p -m 0755 /home/tmp_$Device
	chmod 0755 /home/tmp_$Device
done

if [[ "$MakeUsers" == yes ]]; then
	files="samba/smbpasswd shadow passwd group"
	for file in $files; do
		if ! BlacklistConfFiles "/etc/$file" ;then
			mv -f /etc/$file.$$ /etc/$file
		fi
	done

	# Only now we have the copies in place of the originals so we can add users and set ownerships
	addgroup public &>/dev/null
	for Users in $ListOfUsers; do
		PlutoUserID=$(Field 1 "$Users")
		UserName=$(Field 2 "$Users" | tr 'A-Z' 'a-z' | tr -dc "a-z0-9-")
		adduser pluto_$UserName public &>/dev/null
		chown -R pluto_$UserName.pluto_$UserName /home/user_$PlutoUserID/
	done

	if [[ "$(pidof smbd)" != "" ]] ;then
		invoke-rc.d samba reload
	fi
fi

chmod -R 2770 "$BaseDir"/user_* 2>/dev/null
chmod -R 2775 "$BaseDir"/public 2>/dev/null
chgrp -R public "$BaseDir"/public
echo "$(date -R) chgrp -R public $BaseDir/public" >> /var/log/pluto/SetupUsers_Homes.log

## ReAdd the sambahelper user to smbpasswd
if [[ -r /usr/pluto/var/sambaCredentials.secret ]] ;then
	smbpass=$(cat /usr/pluto/var/sambaCredentials.secret | grep '^password' | cut -d '=' -f2)
	smbpass=$(/usr/pluto/bin/smbpass.pl $smbpass)

	smbuser=$(cat /usr/pluto/var/sambaCredentials.secret | grep '^user' | cut -d '=' -f2)
	smbuserid=$(id -u $smbuser)

	if [[ $(grep "^$smbuser:" /etc/samba/smbpasswd) == "" ]] ;then
		NeedToRestart="true"
	fi

	if ! BlacklistConfFiles '/etc/samba/smbpasswd' ;then
		echo "$smbuser:$LinuxUserID:$smbpass:[U          ]:LCT-00000001:,,," >> /etc/samba/smbpasswd 
	fi

	if [[ "$NeedToRestart" == "true" &&  "$(pidof smbd)" != "" ]] ;then
		invoke-rc.d samba restart
	fi
fi

## Rebuild NIS cache
make -C /var/yp

/usr/pluto/bin/UpdateMediaDaemonControl.sh  -disable
/usr/pluto/bin/UpdateMediaDaemonControl.sh  -enable
