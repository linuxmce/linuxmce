#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

# Initial Release
# Author - Rob Woodward
# Changes - phenigma (phenigma@hotmail.com)
#	integrated into lmce, pakcaged, installation common to lmce

transmissionSettings="/etc/transmission-daemon/settings.json"
apachesettings="/etc/apache2/sites-available/LinuxMCE-ssl"

changeSetting()
{
        #create a back-up of the current settings file
        backup="_"`date '+%Y%m%d%k%M%S'`".bak"
        oldSetting=$(grep "\"$1\":" $transmissionSettings|awk -F\: '{print $2}'|sed -e 's|\,||g' -e 's|^ *||g' -e 's| *$||g' -e 's|"|\\"|g')

        if [ `echo ${oldSetting:1:1}` == '"' ]
        then
                sed -i$backup "s|\"$1\": $oldSetting,|\"$1\": \"$2\",|" $transmissionSettings
        else
                sed -i$backup "s|\"$1\": $oldSetting,|\"$1\": $2,|" $transmissionSettings
        fi

        echo $transmissionSettings$backup
}

addline()
{
 sed -i "/$1/i$2" $3
}


DBName='transmission_links'

#Set up the database
mysql -e "CREATE DATABASE IF NOT EXISTS $DBName;"
mysql -e "CREATE TABLE IF NOT EXISTS Torrent_Links (Link_ID int NOT NULL AUTO_INCREMENT, Torrent_Location TEXT NOT NULL, Rar_File VARCHAR(250) NULL, Link_Location TEXT NOT NULL, Link_Name VARCHAR(100), Link_Type VARCHAR(10) NOT NULL, Link_Status VARCHAR(10) NULL, PRIMARY KEY (Link_ID));" $DBName

#Adding the user
mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE ON transmission_links.* TO 'transmission'@'localhost' IDENTIFIED BY '';"
mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE ON transmission_links.* TO 'transmission'@'127.0.0.1' IDENTIFIED BY '';"
mysql -e "GRANT SELECT, INSERT, UPDATE, DELETE ON transmission_links.* TO 'transmission'@'192.168.80.*' IDENTIFIED BY '';"
mysql -e "FLUSH PRIVILEGES;"



#Add the debian-transmission user to the public group, Create the torrent directories and change their ownership to the transmission user
usermod -a -G public debian-transmission


completeDir="/home/public/torrents/complete"
incompleteDir="/home/public/torrents/incomplete"
mkdir -p $completeDir
mkdir -p $incompleteDir

chown debian-transmission:debian-transmission $completeDir
chown debian-transmission:debian-transmission $incompleteDir

#Now edit the transmission settings file to complete installation
service transmission-daemon stop

#Change each of the settings
changeSetting "download-dir" "$completeDir"
#Add a slight pause to make sure we don't overwrite the original back-up file, then followinf aren't important to back-up
sleep 2
backup=$(changeSetting "incomplete-dir" "$incompleteDir")
rm $backup

sleep 2
backup=$(changeSetting "incomplete-dir-enabled" true)
rm $backup

if [ `grep -c "script-torrent-done-enabled" "$transmissionSettings"` -gt 0 ]
then
	sleep 2
	backup=$(changeSetting "script-torrent-done-enabled" true)
	rm $backup
else
	addline "}" "\ \ \ \ \"script-torrent-done-enabled\": true" "$transmissionSettings"
fi

if [ `grep -c "script-torrent-done-filename" "$transmissionSettings"` -gt 0 ]
then
	sleep 2
	backup=$(changeSetting "script-torrent-done-filename" "/usr/pluto/bin/transmission-settings.sh torrentComplete \$TR_TORRENT_NAME")
	rm $backup
else
	addline "}" "\ \ \ \ \"script-torrent-done-filename\": \"/usr/pluto/bin/transmission-settings.sh torrentComplete \$TR_TORRENT_NAME\"" "$transmissionSettings"
fi

#Re-start the transmission-daemon service
service transmission-daemon start

if [ ! `grep -c "$apachesettings" "#####  Transmission-daemon lines #####"` ]
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
