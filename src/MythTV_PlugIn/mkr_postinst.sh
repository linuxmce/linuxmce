#!/bin/bash
. /usr/pluto/bin/Utils.sh

AddBookmark "http://dcerouter/mythweb/" "MythWeb"

adduser mythtv public || :
mkdir -p /root/.mythtv && touch /root/.mythtv/ignoregroup || :

#Re-run SetupUsers_Homes PlutoStorageDevices to make sure that the mythtv user gets added to each user's group
/usr/pluto/bin/SetupUsers_Homes.sh
/usr/pluto/bin/StorageDevices_Setup.sh
