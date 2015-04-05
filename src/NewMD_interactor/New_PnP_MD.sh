#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/SQL_Ops.sh

Interactor_Port=7238

RemoteIP="$1"
RemoteMAC="$2"
MD_Device="$3"

bright="[1m"
color="[1;31m"
normal="[0m"

FeedbackMD()
{
	echo "msg $1" | nc -n -q 0 "$RemoteIP" "$Interactor_Port"
}

Logging "$TYPE" "$SEVERITY_NORMAL" "New PnP MD" "Setting up new PnP MD with ID '$MD_Device', MAC address '$RemoteMAC' and provisional IP '$RemoteIP'"
echo "deviceid $MD_Device" | nc -n -q 0 "$RemoteIP" "$Interactor_Port"

Q="
	UPDATE Device
	SET IPaddress=''
	WHERE PK_Device='$MD_Device'
"
RunSQL "$Q"

MD_IP=$(/usr/pluto/bin/PlutoDHCP.sh -d "$MD_Device" -a)
Logging "$TYPE" "$SEVERITY_NORMAL" "Assigned permanent IP '$MD_IP' to device '$MD_Device'"
FeedbackMD "Assigned permanent IP '$MD_IP' to device '$MD_Device'"

WaitLock "NewPnPMD" "NewPnPMD$MD_Device"
Logging "$TYPE" "$SEVERITY_NORMAL" "Running diskless MD setup command suite (device '$MD_Device')"

fail=0
# Note: keep this in sync with /var/www/lmce-admin/operations/logs/executeLog.php, case 1
FeedbackMD "Running Diskless_Setup.sh"
if ! /usr/pluto/bin/Diskless_Setup.sh; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "Diskless Setup failed"
	FeedbackMD "${color}${bright}Diskless Setup failed${normal}"
	fail=1
fi

FeedbackMD "Running Disked_Setup.sh"
if ! /usr/pluto/bin/Disked_Setup.sh; then
	Logging "$TYPE" "$SEVERITY_CRITICAL" "Disked Setup failed"
	FeedbackMD "${color}${bright}Diskless Setup failed${normal}"
	fail=2
fi

if [[ "$fail" != "0" ]] ; then
	Unlock "NewPnPMD" "NewPnPMD$MD_Device"
	exit $fail
fi

FeedbackMD "Running DHCP_config.sh"
/usr/pluto/bin/DHCP_config.sh

FeedbackMD "Running Diskless_ExportsNFS.sh"
/usr/pluto/bin/Diskless_ExportsNFS.sh

# End of list to be kept in sync
Logging "$TYPE" "$SEVERITY_NORMAL" "Finished running diskless MD setup command suite (device '$MD_Device')"
Unlock "NewPnPMD" "NewPnPMD$MD_Device"

Logging "$TYPE" "$SEVERITY_NORMAL" "Rebooting new MD"
FeedbackMD "Rebooting in 5 seconds..."
sleep 1
FeedbackMD "Rebooting in 4 seconds..."
sleep 1
FeedbackMD "Rebooting in 3 seconds..."
sleep 1
FeedbackMD "Rebooting in 2 seconds..."
sleep 1
FeedbackMD "Rebooting in 1 seconds..."
sleep 1
FeedbackMD "Rebooting"
echo "reboot" | nc -n "$RemoteIP" "$Interactor_Port"
