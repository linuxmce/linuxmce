#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

echo "$(date -R) Starting Core Services - config device changes" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/Config_Device_Changes.sh >> /var/log/pluto/StartCoreServices.log 2>&1

echo "$(date -R) Starting Core Services - start orbitergen " >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/Start_OrbiterGen.sh >> /var/log/pluto/StartCoreServices.log 2>&1

/usr/pluto/bin/UpdateMediaDaemonControl.sh -enable

Q="SELECT PK_Device FROM Device"
Devices=$(RunSQL "$Q")
/usr/pluto/bin/sync_pluto2amp.pl $Devices

echo "$(date -R) Starting Core Services - starting dcerouter" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/Start_DCERouter.sh >> /var/log/pluto/StartCoreServices.log 2>&1
echo "$(date -R) Starting Core Services - done starting dcerouter" >> /var/log/pluto/StartCoreServices.log 2>&1

#wake mds
echo "$(date -R) Starting Core Services - waking up M/Ds" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/WakeMDs.sh >> /var/log/pluto/StartCoreServices.log 2>&1
echo "$(date -R) Starting Core Services - done waking up M/Ds" >> /var/log/pluto/StartCoreServices.log 2>&1

#check if there are raids from previous install (will run only once)
echo "$(date -R) Starting Core Services - checking for RAIDs" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/checkforRaids.sh >> /var/log/pluto/StartCoreServices.log 2>&1
echo "$(date -R) Starting Core Services - done checking for RAIDs" >> /var/log/pluto/StartCoreServices.log 2>&1

