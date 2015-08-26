#!/bin/bash

. /usr/pluto/bin/SQL_Ops.sh

#echo "$(date -R) Starting Core Services - config device changes" >> /var/log/pluto/StartCoreServices.log 2>&1
#bash -x /usr/pluto/bin/Config_Device_Changes.sh >> /var/log/pluto/StartCoreServices.log 2>&1

# Start DCERouter before doing the orbitergen, otherwise LMCE Launch Manager will try starting devices which
# end up being disabled due to DCERouter not yet running.
echo "$(date -R) Starting Core Services - starting dcerouter" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/Start_DCERouter.sh >> /var/log/pluto/StartCoreServices.log 2>&1
echo "$(date -R) Starting Core Services - done starting dcerouter" >> /var/log/pluto/StartCoreServices.log 2>&1

#echo "$(date -R) Starting Core Services - start orbitergen " >> /var/log/pluto/StartCoreServices.log 2>&1
#bash -x /usr/pluto/bin/Start_OrbiterGen.sh >> /var/log/pluto/StartCoreServices.log 2>&1

Q="SELECT Enabled FROM Device_StartupScript WHERE FK_Device=$PK_Device AND FK_StartupScript=61"
StartUpdateMedia=$(RunSQL "$Q")
if [ "$StartUpdateMedia" -ne "0" ] ; then
/usr/pluto/bin/UpdateMediaDaemonControl.sh -enable
fi

Q="SELECT PK_Device FROM pluto_main.Device LEFT JOIN pluto_main.Device_DeviceData ON pluto_main.Device.PK_Device = pluto_main.Device_DeviceData.FK_Device where pluto_main.Device_DeviceData.FK_DeviceData in (31,29) group by PK_Device"
Devices=$(RunSQL "$Q")
/usr/pluto/bin/sync_pluto2amp.pl $Devices

#wake mds
echo "$(date -R) Starting Core Services - waking up M/Ds" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/WakeMDs.sh >> /var/log/pluto/StartCoreServices.log 2>&1
echo "$(date -R) Starting Core Services - done waking up M/Ds" >> /var/log/pluto/StartCoreServices.log 2>&1

#check if there are raids from previous install (will run only once)
echo "$(date -R) Starting Core Services - checking for RAIDs" >> /var/log/pluto/StartCoreServices.log 2>&1
bash -x /usr/pluto/bin/checkforRaids.sh >> /var/log/pluto/StartCoreServices.log 2>&1
echo "$(date -R) Starting Core Services - done checking for RAIDs" >> /var/log/pluto/StartCoreServices.log 2>&1

