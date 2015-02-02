#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

# Initial Release
# Author - Rob Woodward
# Changes - phenigma (phenigma@hotmail.com)
#       integrated into lmce, pakcaged, installation common to lmce

transmissionSettings="/etc/transmission-daemon/settings.json"
apachesettings="/etc/apache2/sites-available/LinuxMCE-ssl.conf"

addline()
{
 sed -i "/$1/i$2" "$3"
}

changeSetting()
{
	if [ $(grep -c "$1" "$transmissionSettings") -gt 0 ]; then
		# if setting exists, we change it
		oldSetting=$(grep "\"$1\":" "$transmissionSettings"|awk -F\: '{print $2}'|sed -e 's|\,||g' -e 's|^ *||g' -e 's| *$||g' -e 's|"|\\"|g')
		if [ $(echo ${oldSetting:1:1}) == '"' ]; then
			sed -i "s|\"$1\": $oldSetting,|\"$1\": \"$2\",|" "$transmissionSettings"
		else
			sed -i "s|\"$1\": $oldSetting,|\"$1\": $2,|" "$transmissionSettings"
		fi
	else
		# add the line if the setting does not already exist
		addline "}" "\ \ \ \ \"$1\": $2" "$transmissionSettings"
	fi
}

DBName='transmission_links'

#Set up the database
mysql -e "CREATE DATABASE IF NOT EXISTS $DBName;"
mysql -e "CREATE TABLE IF NOT EXISTS Torrent_Links (Link_ID int NOT NULL AUTO_INCREMENT, Torrent_Location TEXT NOT NULL, Rar_File VARCHAR(250) NULL, Link_Location TEXT NOT NULL, Link_Name VARCHAR(100), Link_Type VARCHAR(10) NOT NULL, Link_Status VARCHAR(10) NULL, PRIMARY KEY (Link_ID));" $DBName

#Adding the user
mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE ON $DBName.* TO 'transmission'@'localhost' IDENTIFIED BY '';"
mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE ON $DBName.* TO 'transmission'@'127.0.0.1' IDENTIFIED BY '';"
mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE ON $DBName.* TO 'transmission'@'192.168.80.%' IDENTIFIED BY '';"
mysql -e "FLUSH PRIVILEGES;"



#Add the debian-transmission user to the public group, Create the torrent directories and change their ownership to the transmission user
usermod -a -G public debian-transmission


completeDir="/home/public/torrents/complete"
incompleteDir="/home/public/torrents/incomplete"
mkdir -p "$completeDir"
mkdir -p "$incompleteDir"

chown debian-transmission:debian-transmission "$completeDir"
chown debian-transmission:debian-transmission "$incompleteDir"

#Now edit the transmission settings file to complete installation
service transmission-daemon stop

#create a back-up of the current settings file
if [[ -e "$transmissionSettings" ]]; then
	cp -a "$transmissionSettings" "$transmissionSettings".lmcebackup
fi

#Change each of the settings
changeSetting "download-dir" "$completeDir"
changeSetting "incomplete-dir" "$incompleteDir"
changeSetting "incomplete-dir-enabled" "true"
changeSetting "script-torrent-done-enabled" "true"
changeSetting "script-torrent-done-filename" "/usr/pluto/bin/transmission-settings.sh torrentComplete \$TR_TORRENT_NAME"
changeSetting "rpc-whitelist" "127.0.0.1,192.168.80.*"

#Re-start the transmission-daemon service
service transmission-daemon start

if [ ! $(grep -c "#####  Transmission-daemon lines #####" "$apachesettings") ]
then
	#Finally, sort out the Apache Settings
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ #################################################################" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ #####  Transmission-daemon lines ################################" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ RewriteEngine On" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ RewriteOptions Inherit" "$apachesettings"
	addline "<\/VirtualHost>" "\ " "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ ProxyRequests Off" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ <Proxy *>" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ Order Allow,Deny" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ Allow from all" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ <\/Proxy>" "$apachesettings"
	addline "<\/VirtualHost>" "\ " "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ # Redirect requests to \/transmission to \/transmission\/web" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ RewriteRule \/transmission[\/]?$ \/transmission\/web [R=permanent]" "$apachesettings"
	addline "<\/VirtualHost>" "\ " "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ # Forward all requests for \/transmission to transmission-daemon" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ ProxyPass \/transmission http:\/\/127.0.0.1:9091\/transmission" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ ProxyPassReverse \/transmission http:\/\/127.0.0.1:9091\/transmission" "$apachesettings"
	addline "<\/VirtualHost>" "\ " "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ ######## End Transmission-daemon lines ##########################" "$apachesettings"
	addline "<\/VirtualHost>" "\ \ \ \ \ \ \ \ #################################################################" "$apachesettings"

	#Add Proxy pass module in
	a2enmod proxy
	a2enmod proxy_http

	#Restart apache to take on the new settings
	service apache2 restart
fi

exit 0

