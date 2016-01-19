#!/bin/bash
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICECATEGORY_Core_CONST=7

AddBookmark "http://dcerouter/mythweb/" "MythWeb"

adduser mythtv public || :
rm -rf /root/.mythtv || :
ln -s /etc/mythtv /root/.mythtv && touch /root/.mythtv/ignoregroup || :
ln -sf /etc/mythtv /home/mythtv/.mythtv || :
chown mythtv /etc/mythtv || :

#Alter mythconverg.storagegroup - the default VARCHAR(32) is too short for some LMCE drive names to fully be displayed
Q="ALTER TABLE storagegroup MODIFY dirname VARCHAR(203) NOT NULL, MODIFY groupname VARCHAR(64) NOT NULL"
UseDB "mythconverg"
RunSQL "$Q"

#Alter mythconverg.settings to change the auto expire value to 10 GB to avoid conflicts with LMCE's low disk space warning
Q="UPDATE settings SET data=10 where value='AutoExpireExtraSpace'"
UseDB "mythconverg"
RunSQL "$Q"

#Alter mythconverg.settings to force Media Directors to stream recordings from the backend on the core to avoid errors with non-system drive storage groups
Q="UPDATE settings SET data=1 where value='AlwaysStreamFiles'"
UseDB "mythconverg"
RunSQL "$Q"

#Alter mythconverg.settings to force Media Directors to delete files slowly for performance reasons
Q="UPDATE settings SET data=1 where value='TruncateDeletesSlowly'"
UseDB "mythconverg"
RunSQL "$Q"

Q="SELECT IPAddress FROM Device JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate where FK_DeviceCategory='${DEVICECATEGORY_Core_CONST}'"
UseDB pluto_main
R=$(RunSQL "$Q")
if [ -n "$R" ] ; then
	#Alter mythconverg.settings to force MasterServerIP to the core's internal NIC
	Q="UPDATE settings SET data='${R}' where value='MasterServerIP'"
	UseDB "mythconverg"
	RunSQL "$Q"
fi

#Re-run SetupUsers_Homes PlutoStorageDevices to make sure that the mythtv user gets added to each user's group
/usr/pluto/bin/SetupUsers_Homes.sh
/usr/pluto/bin/StorageDevices_Setup.sh

#hack to fix broken mythweb package
#FIXME: remove once mythtv packages are fixed
chown -R www-data: /var/www/mythweb || :

# make sure that mythweb hasn't taken over web admin
a2dissite default-mythbuntu || /bin/true
a2dissite 000-default-mythbuntu || /bin/true

#Run mythtv-setup.sh to make sure storage groups get added. -tschak
/usr/pluto/bin/mythtv_setup.pl

service apache2 force-reload || :
