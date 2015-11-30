#!/bin/bash

DEVICEDATA_Hostname=188
DEVICEDATA_Domainname=187

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Utils.sh

# Host- and domainname
H="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DEVICEDATA_Hostname"
D="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=$DEVICEDATA_Domainname"
HostName=$(RunSQL "$H")
DomainName=$(RunSQL "$D")

# Site files
default=LinuxMCE.conf
defaultssl=LinuxMCE-ssl.conf

PHP_CONFIG_FILE=
if [[ -e /etc/php5/apache2/php.ini ]] ;then
	PHP_CONFIG_FILE=/etc/php5/apache2/php.ini
else
	PHP_CONFIG_FILE=/etc/php4/apache2/php.ini
fi

# clean out old pluto-admin links
rm -f /var/www/pluto-admin/floorplans
rm -f /var/www/pluto-admin/users
rm -f /var/www/pluto-admin/mediapics
rm -f /var/www/pluto-admin/rooms
rm -f /var/www/pluto-admin/scenarios
rm -f /var/www/pluto-admin/orbiter_bg
rm -rf /var/www/pluto-admin/security_images
rm -rf /var/www/pluto-admin/cached

mkdir -p /home/pluto/floorplans
chmod -R 777 /home/pluto/floorplans
chown -R www-data.www-data /home/pluto/floorplans
rm -f /var/www/lmce-admin/floorplans 2>/dev/null
[[ -e /usr/pluto/orbiter/floorplans ]] || ln -sn /home/pluto/floorplans /usr/pluto/orbiter/floorplans
[[ -e /var/www/lmce-admin/floorplans ]] || ln -sn /usr/pluto/orbiter/floorplans/ /var/www/lmce-admin/

mkdir -p /usr/pluto/orbiter/users
chmod -R 777 /usr/pluto/orbiter/users
chown -R www-data.www-data /usr/pluto/orbiter/users
rm -f /var/www/lmce-admin/users
ln -s /usr/pluto/orbiter/users/ /var/www/lmce-admin/

mkdir -p /home/mediapics
chmod -R 777 /home/mediapics
rm -f /var/www/lmce-admin/mediapics
ln -s /home/mediapics /var/www/lmce-admin/mediapics

mkdir -p /usr/pluto/orbiter/rooms
chmod -R 777 /usr/pluto/orbiter/rooms
rm -f /var/www/lmce-admin/rooms
ln -s /usr/pluto/orbiter/rooms /var/www/lmce-admin/rooms

mkdir -p /usr/pluto/orbiter/scenarios
chmod -R 777 /usr/pluto/orbiter/scenarios
rm -f /var/www/lmce-admin/scenarios
ln -s /usr/pluto/orbiter/scenarios /var/www/lmce-admin/scenarios

mkdir -p /usr/pluto/orbiter/orbiter_bg
chmod -R 777 /usr/pluto/orbiter/orbiter_bg
rm -f /var/www/lmce-admin/orbiter_bg
ln -s /usr/pluto/orbiter/orbiter_bg /var/www/lmce-admin/orbiter_bg

# Adding contact images folder - tschak
mkdir -p /usr/pluto/orbiter/contacts
chmod -R 777 /usr/pluto/orbiter/contacts
rm -f /var/www/lmce-admin/contacts
ln -s /usr/pluto/orbiter/contacts /var/www/lmce-admin/contacts

mkdir -p /var/www/lmce-admin/security_images
chmod -R 777 /var/www/lmce-admin/security_images

chmod 777 /var/www/lmce-admin/cached
touch /etc/pluto-callerid.conf
chmod 777 /etc/pluto-callerid.conf

mkdir -p /var/log/pluto
touch /var/log/pluto/webExecLog.log
chown www-data.www-data /var/log/pluto/webExecLog.log

mkdir -p /home/coverartscan
chown www-data.www-data /home/coverartscan
ln -snf /home/coverartscan /var/www/lmce-admin/coverartscan || :
rm -f /var/www/pluto-admin/coverartscan

mv /var/www/lmce-admin/grabcoverart.sh /usr/pluto/bin 2>/dev/null|| /bin/true

# enable rewrite module
a2enmod rewrite || /bin/true

# disable old pluto site
a2dissite pluto 2>/dev/null|| /bin/true

# disable default site that ships with apache
a2dissite default 2>/dev/null|| /bin/true
a2dissite default-ssl 2>/dev/null|| /bin/true
a2dissite 000-default 2>/dev/null|| /bin/true
a2dissite 000-default-ssl 2>/dev/null|| /bin/true

# make sure that mythweb hasn't taken over web admin
a2dissite 000-default-mythbuntu 2>/dev/null|| /bin/true
a2dissite default-mythbuntu 2>/dev/null|| /bin/true

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
## @FileType: Pluto Sectioned Config File ##
@SSL_START
<VirtualHost _default_:@PORT>
	ServerAdmin webmaster@$HostName.$DomainName
	ServerName $HostName.$DomainName
		
	DocumentRoot /var/www/
	<Directory />
		Options +FollowSymLinks
		AllowOverride None
	</Directory>
	<Directory /var/www/>
		Options +Indexes +FollowSymLinks +MultiViews
		AllowOverride All
		Order allow,deny
		allow from all
		RedirectMatch ^/$ /lmce-admin/
	</Directory>

	ScriptAlias /cgi-bin/ /usr/lib/cgi-bin/
	<Directory \"/usr/lib/cgi-bin\">
		AllowOverride None
		Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
		Order allow,deny
		Allow from all
	</Directory>

	Alias /fax-incoming/ /home/fax/incoming/
    Alias /fax-outgoing/ /home/fax/outgoing/

	Alias /voicemail/ /var/spool/asterisk/voicemail/
	<Directory \"/var/spool/asterisk/voicemail\">
		AllowOverride None
		Options -Indexes -MultiViews
		Order allow,deny
		Allow from all
	</Directory>
	
	ErrorLog /var/log/apache2/error.log
	LogLevel warn

	CustomLog /var/log/apache2/@LOGFILE combined
	ServerSignature On
	
	Alias /icons/ \"/usr/share/apache2/icons/\"
	<Directory \"/usr/share/apache2/icons\">
	    Options +Indexes +MultiViews
	    AllowOverride None
	    Order allow,deny
	    Allow from all
	</Directory>
## BEGIN : SSL
## END : SSL
</VirtualHost>
@SSL_END
"
ssl="
	SSLEngine on
	SSLCertificateFile    /etc/ssl/certs/server.crt
	SSLCertificateKeyFile /etc/ssl/private/server.key
    SSLOptions +FakeBasicAuth +ExportCertData +StrictRequire
    <FilesMatch \"\\\.(cgi|shtml|phtml|php)$\">
		SSLOptions +StdEnvVars
    </FilesMatch>
    <Directory /usr/lib/cgi-bin>
		SSLOptions +StdEnvVars
    </Directory>
     BrowserMatch \"MSIE [2-6]\" nokeepalive ssl-unclean-shutdown downgrade-1.0 force-response-1.0
     BrowserMatch \"MSIE [17-9]\" ssl-unclean-shutdown

"

# Create default website config file
echo "$Site" >/etc/apache2/sites-available/$default
sed -i "s/@SSL_START//" /etc/apache2/sites-available/$default
sed -i "s/@SSL_END//" /etc/apache2/sites-available/$default
sed -i "s/@PORT/80/" /etc/apache2/sites-available/$default
sed -i "s/@LOGFILE/access.log/" /etc/apache2/sites-available/$default
a2ensite $default

# Create ssl website config file and enable ssl
echo "$Site" >/etc/apache2/sites-available/$defaultssl
sed -i "s/@SSL_START/<IfModule mod_ssl.c>/" /etc/apache2/sites-available/$defaultssl
PopulateSection "/etc/apache2/sites-available/$defaultssl" "SSL" "$ssl"
sed -i "s/@SSL_END/<\/IfModule>/" /etc/apache2/sites-available/$defaultssl
sed -i "s/@PORT/443/" /etc/apache2/sites-available/$defaultssl
sed -i "s/@LOGFILE/ssl_access.log/" /etc/apache2/sites-available/$defaultssl
a2ensite $defaultssl
a2enmod ssl

if ! BlacklistConfFiles '/etc/apache2/apache2.conf' ;then
	if [ ! -e '/etc/apache2/apache2.conf.pbackup' ] && [ -e '/etc/apache2/apache2.conf' ] ;then
		cp /etc/apache2/apache2.conf /etc/apache2/apache2.conf.pbackup || :
	fi

	if [ -d /etc/apache2/conf.d ] ; then
		ConfDir="/etc/apache2/conf.d"
	elif [ -d /etc/apache2/conf-available ] ; then
		ConfDir="/etc/apache2/conf-available"
	fi
	if ! grep ServerName "${ConfDir}/lmce.conf" >/dev/null ; then
		if [[ -z "${HostName}" ]] ; then
			HostName="dcerouter"
		fi
		echo "ServerName ${HostName}" > "${ConfDir}/lmce.conf"
	fi
	a2enconf lmce || :
fi

if ! BlacklistConfFiles '/etc/apache2/ports.conf' ;then
	if [ ! -e '/etc/apache2/ports.conf.pbackup' ] && [ -e '/etc/apache2/ports.conf' ] ;then
		cp /etc/apache2/ports.conf /etc/apache2/ports.conf.pbackup || :
	fi

echo "Listen 80
Listen 8080
Listen 443" >/etc/apache2/ports.conf
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

[[ -f /etc/wap.conf ]] || : >/etc/wap.conf
chown www-data.root /etc/wap.conf
chmod 664 /etc/wap.conf

if ! BlacklistConfFiles "$PHP_CONFIG_FILE" ;then
	#Make php.ini backup
	if [ ! -e "$PHP_CONFIG_FILE.pbackup" ] ;then
		cp "$PHP_CONFIG_FILE" "$PHP_CONFIG_FILE.pbackup" || :
	fi
				
	#adjust PHP memory limit
	sed -i 's/memory_limit =.*/memory_limit = 512M ;/g' $PHP_CONFIG_FILE
	#adjust PHP session max lifetime
	sed -i 's/^session.gc_maxlifetime = 1440$/session.gc_maxlifetime = 144000/' $PHP_CONFIG_FILE
	#adjust PHP max execution time
	sed -i 's/max_execution_time =.*/max_execution_time = 120 ;/g' $PHP_CONFIG_FILE
	#adjust PHP maximum upload filesize
	sed -i 's/upload_max_filesize =.*/upload_max_filesize = 16M ;/g' $PHP_CONFIG_FILE
	#make sure short open tags are acceptable
	sed -i 's/short_open_tag =.*/short_open_tag = On ;/g' $PHP_CONFIG_FILE
fi

# (re)enable SSL support in apache using a self-signed cert

# test for existing cert before doing anything
if [ ! -e /etc/ssl/certs/server.crt ]; then

cd /tmp
cat > config <<EOF
[ req ]
default_bits           = 1024
default_keyfile        = server.csr
distinguished_name     = req_distinguished_name
prompt                 = no
output_password        = password

[ req_distinguished_name ]
OU                     = Web Admin
CN                     = LinuxMCE
EOF

#create cert encryption key
openssl genrsa -des3 -passout pass:password -out server.key.enc 1024 
openssl rsa -in server.key.enc -passin pass:password -out server.key

#create cert request
openssl req -new -config config -out server.csr

#create and sign the cert
openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt

#install the key and cert
cp server.crt /etc/ssl/certs/
cp server.key /etc/ssl/private/

#do some cleanup
rm config
rm server.key
rm server.key.enc
rm server.crt
rm server.csr
fi

service apache2 force-reload || :

