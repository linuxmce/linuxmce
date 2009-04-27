#!/bin/bash
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Config_Ops.sh

AddBookmark "http://dcerouter/mythweb/" "MythWeb"

adduser mythtv public || :
mkdir -p /root/.mythtv && touch /root/.mythtv/ignoregroup || :

#Alter mythconverg.storagegroup - the default VARCHAR(32) is too short for some LMCE drive names to fully be displayed
Q="ALTER TABLE storagegroup MODIFY groupname VARCHAR(64)"
UseDB "mythconverg"
RunSQL "$Q"

#Re-run SetupUsers_Homes PlutoStorageDevices to make sure that the mythtv user gets added to each user's group
/usr/pluto/bin/SetupUsers_Homes.sh
/usr/pluto/bin/StorageDevices_Setup.sh
