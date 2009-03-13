#!/bin/bash

cd /var/www
current_version="$(</var/www/download/current_version)"
rm *.tar.gz
tar -xzvf /home/uploads/everything.tar.gz
#                                                                                          100%
#-rw-r--r-- root/root    953671 2005-02-04 11:50:23 plutovip-com.tar.gz
#-rw-r--r-- root/root    563526 2005-02-04 11:50:30 pluto-support.tar.gz
#-rw-r--r-- root/root   4796539 2005-02-04 11:50:29 trunk_doc.tar.gz
#-rw-r--r-- root/root   5954381 2005-02-04 11:50:25 plutohome-com.tar.gz
#-rw-r--r-- root/root    236193 2005-02-04 11:50:32 dumps.tar.gz
#-rw-r--r-- root/root   4691444 2005-02-02 03:58:14 pluto-support-modified.tar.gz

# pluto-support-modified.tar.gz
for i in plutovip-com.tar.gz pluto-support.tar.gz trunk_doc.tar.gz plutohome-com.tar.gz dumps.tar.gz pluto-admin.tar.gz; do
	echo "Extracting $i"
	tar -xzvf "$i"
done
cp /var/www/lmce-admin_include_config_config.inc.php /var/www/lmce-admin/include/config/config.inc.php
cp /var/www/lmce-admin_globalconfig.inc.php /var/www/lmce-admin/globalconfig.inc.php
rm -f /var/www/plutohome-com/plutoadmin
ln -s /var/www/pluto-admin/ /var/www/plutohome-com/plutoadmin

mysql -uroot -pmoscow70bogata pluto_main < pluto_main.dump
mysql -uroot -pmoscow70bogata plutovip < plutovip.dump
mysql -uroot -pmoscow70bogata pluto_website < pluto_website.dump
sh -x /var/www/UpdateForumsMantis.sh

cd /usr/pluto/lib
tar zxvf /var/www/so.tar.gz
echo delete this when I confirm update so works--it keeps untarring to the wrong directory!!!!!
read

cd /
tar zxvf /var/www/bins.tar.gz
mv /home/MakeRelease/trunk/src/ConfirmDependencies/ConfirmDependencies /usr/pluto/bin
mv /home/MakeRelease/trunk/src/sqlCVS/sqlCVS /usr/pluto/bin
Q="select PK_Version from Version WHERE PK_Version<>1 ORDER BY date desc, PK_Version limit 1"
last_version=$(echo "$Q;" | mysql -u root -pmoscow70bogata -N pluto_main)

Q="select VersionName from Version WHERE PK_Version=$last_version"
last_version_name=$(echo "$Q;" | mysql -u root -pmoscow70bogata -N pluto_main)

Q="UPDATE Package_Source INNER JOIN Package ON FK_Package=PK_Package SET Version='$last_version_name' WHERE FK_Manufacturer=1"
echo "$Q"
echo "$Q;" | mysql -u root -pmoscow70bogata -N pluto_main

echo "Press a key to check versions"
read
echo "SELECT Version FROM Package_Source JOIN Package ON FK_Package=PK_Package WHERE FK_Manufacturer=1" | mysql -A -u root -pmoscow70bogata pluto_main

/usr/pluto/bin/listsync.sh

cd /var/www
cdmd5="$(</var/www/installation-cd.$version_name.md5)"
sed "s/****KICK_START_NAME****/installation-cd.$version_name.iso/g" /var/www/plutohome-com/client/step8a.php > /tmp/step8a.tmp
sed "s/****MD5SUM****/$cdmd5/g" /tmp/step8a.tmp /tmp/step8a.php
mv /tmp/step8a.php /var/www/plutohome-com/client/step8a.php
echo "Confirm cd version is installation-cd.$version_name.iso and md5"
cat /var/www/installation-cd.$version_name.md5
md5sum /var/www/installation-cd.$version_name.iso


