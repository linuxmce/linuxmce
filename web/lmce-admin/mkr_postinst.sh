#!/bin/bash

. /usr/pluto/bin/Utils.sh

PHP_CONFIG_FILE=
if [[ -e /etc/php5/apache2/php.ini ]] ;then
	PHP_CONFIG_FILE=/etc/php5/apache2/php.ini
else
	PHP_CONFIG_FILE=/etc/php4/apache2/php.ini
fi

mkdir -p /home/pluto/floorplans
chmod -R 777 /home/pluto/floorplans
chown -R www-data.www-data /home/pluto/floorplans
rm -f /var/www/pluto-admin/floorplans 2>/dev/null
[[ -e /usr/pluto/orbiter/floorplans ]] || ln -sn /home/pluto/floorplans /usr/pluto/orbiter/floorplans
[[ -e /var/www/pluto-admin/floorplans ]] || ln -sn /usr/pluto/orbiter/floorplans/ /var/www/pluto-admin/

mkdir -p /usr/pluto/orbiter/users
chmod -R 777 /usr/pluto/orbiter/users
chown -R www-data.www-data /usr/pluto/orbiter/users
rm -f /var/www/pluto-admin/users
ln -s /usr/pluto/orbiter/users/ /var/www/pluto-admin/

mkdir -p /home/mediapics
chmod -R 777 /home/mediapics
rm -f /var/www/pluto-admin/mediapics
ln -s /home/mediapics /var/www/pluto-admin/mediapics

mkdir -p /usr/pluto/orbiter/rooms
chmod -R 777 /usr/pluto/orbiter/rooms
rm -f /var/www/pluto-admin/rooms
ln -s /usr/pluto/orbiter/rooms /var/www/pluto-admin/rooms

mkdir -p /usr/pluto/orbiter/scenarios
chmod -R 777 /usr/pluto/orbiter/scenarios
rm -f /var/www/pluto-admin/scenarios
ln -s /usr/pluto/orbiter/scenarios /var/www/pluto-admin/scenarios

mkdir -p /usr/pluto/orbiter/orbiter_bg
chmod -R 777 /usr/pluto/orbiter/orbiter_bg
rm -f /var/www/pluto-admin/orbiter_bg
ln -s /usr/pluto/orbiter/orbiter_bg /var/www/pluto-admin/orbiter_bg

mkdir -p /var/www/pluto-admin/security_images
chmod -R 777 /var/www/pluto-admin/security_images

chmod 777 /var/www/pluto-admin/cached
touch /etc/pluto-callerid.conf
chmod 777 /etc/pluto-callerid.conf

mkdir -p /var/log/pluto
touch /var/log/pluto/webExecLog.log
chown www-data.www-data /var/log/pluto/webExecLog.log

mkdir -p /home/coverartscan
chown www-data.www-data /home/coverartscan
ln -snf /home/coverartscan /var/www/pluto-admin/coverartscan || :

mv /var/www/pluto-admin/grabcoverart.sh /usr/pluto/bin || /bin/true

# enable rewrite module
a2enmod rewrite || /bin/true

# disable default site that ships with apache
a2dissite default || /bin/true

#Index=$(grep DirectoryIndex /etc/apache/httpd.conf | sed 's/DirectoryIndex//g; s/^ *//g; s/ *$//g' | grep -v '^#')
#[ -n "$Index" ] || exit 0

#for File in $Index; do
#	[ -e "/var/www/$File" -a "$File" != "index.html" ] && exit 0
#done
#[ -e "/var/www/index.html" ] && ! diff /usr/share/apache/default-configs/apache/intro.html /var/www/index.html &>/dev/null && exit 0
#rm -f /var/www/index.html

#Redirect="<?php header('Location: support/index.php?section=home&package=0'); ?>"
#[ -e /var/www/index.php ] || echo "$Redirect" >/var/www/index.php

Site="
NameVirtualHost *
<VirtualHost *>
	ServerAdmin webmaster@localhost
	
	DocumentRoot /var/www/
	<Directory />
		Options FollowSymLinks
		AllowOverride None
	</Directory>
	<Directory /var/www/>
		Options Indexes FollowSymLinks MultiViews
		AllowOverride All
		Order allow,deny
		allow from all
		# This directive allows us to have apache2's default start page
                # in /apache2-default/, but still have / go to the right place
		RedirectMatch ^/$ /pluto-admin/
	</Directory>

	ScriptAlias /cgi-bin/ /usr/lib/cgi-bin/
	<Directory \"/usr/lib/cgi-bin\">
		AllowOverride None
		Options ExecCGI -MultiViews +SymLinksIfOwnerMatch
		Order allow,deny
		Allow from all
	</Directory>

	ErrorLog /var/log/apache2/error.log

	# Possible values include: debug, info, notice, warn, error, crit,
	# alert, emerg.
	LogLevel warn

	CustomLog /var/log/apache2/access.log combined
	ServerSignature On
	
	Alias /icons/ \"/usr/share/apache2/icons/\"
	<Directory \"/usr/share/apache2/icons\">
	    Options Indexes MultiViews
	    AllowOverride None
	    Order allow,deny
	    Allow from all
	</Directory>

    Alias /doc/ \"/usr/share/doc/\"
    <Directory \"/usr/share/doc/\">
        Options Indexes MultiViews FollowSymLinks
        AllowOverride None
        Order deny,allow
        Deny from all
        Allow from 127.0.0.0/255.0.0.0 ::1/128
    </Directory>

</VirtualHost>
"

echo "$Site" >/etc/apache2/sites-available/pluto
a2ensite pluto

if ! BlacklistConfFiles '/etc/apache2/ports.conf' ;then
	if [ ! -e '/etc/apache2/ports.conf.pbackup' ] && [ -e '/etc/apache2/ports.conf' ] ;then
		cp /etc/apache2/ports.conf /etc/apache2/ports.conf.pbackup || :
	fi
						
echo "Listen 80
Listen 8080" >/etc/apache2/ports.conf
fi

#cmd_alias="Cmnd_Alias	PLUTO_WEBCMD = /usr/pluto/bin/SetupRemoteAccess.sh, /usr/pluto/bin/Network_Firewall.sh, /usr/pluto/bin/SetupUsers.sh, /usr/pluto/bin/Diskless_Setup.sh, /usr/pluto/bin/LaunchRemoteCmd.sh, /usr/pluto/bin/SetTimeZone.sh, /usr/pluto/bin/Update_StartupScrips.sh, /usr/pluto/bin/UpdateMedia"
cmd_alias="Cmnd_Alias	PLUTO_WEBCMD = /usr/pluto/bin/*, /usr/bin/find, /sbin/reboot, /bin/cp, /bin/mv, /bin/rm, /bin/mkdir, /usr/sbin/asterisk"
sudo="www-data	ALL=(root) NOPASSWD:PLUTO_WEBCMD"

awk '!/Cmnd_Alias.*PLUTO_WEBCMD/ && !/www-data.*ALL=\(root\)/' /etc/sudoers >>/etc/sudoers.$$

if ! BlacklistConfFiles '/etc/sudoers' ;then
	if [ ! -e '/etc/sudoers.pbackup' ] ;then
		cp /etc/sudoers /etc/sudoers.pbackup || :
	fi
											
	echo "$cmd_alias" >>/etc/sudoers.$$
	echo "$sudo" >>/etc/sudoers.$$
	mv -f /etc/sudoers.$$ /etc/sudoers
	chmod 440 /etc/sudoers
fi

if ! BlacklistConfFiles "$PHP_CONFIG_FILE" ;then
	    if [ ! -e "$PHP_CONFIG_FILE.pbackup" ] ;then
			        cp "$PHP_CONFIG_FILE" "$PHP_CONFIG_FILE.pbackup" || :
		fi				
		sed -i 's/^session.gc_maxlifetime = 1440$/session.gc_maxlifetime = 144000/' $PHP_CONFIG_FILE
fi

/etc/init.d/apache2 reload

[[ -f /etc/wap.conf ]] || : >/etc/wap.conf
chown www-data.root /etc/wap.conf
chmod 664 /etc/wap.conf

if ! BlacklistConfFiles "$PHP_CONFIG_FILE" ;then
	sed -i 's/memory_limit =.*/memory_limit = 16M ;/g' $PHP_CONFIG_FILE
fi
