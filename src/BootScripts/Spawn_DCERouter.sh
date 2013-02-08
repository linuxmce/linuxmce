#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/PlutoVersion.h
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/LockUtils.sh

trap 'Unlock "DCERouter" "Spawn_DCERouter"' EXIT
Lock "DCERouter" "Spawn_DCERouter" || exit 1

# hack: cleaning lockfile on router start to allow
# local devices to start
# TODO: remove this when correct locking will be implemented
rm -f /usr/pluto/locks/pluto_spawned_local_devices.txt

# The following directory needs to exist, for people who want to
# load plugins without a device template (and the existance of the
# dir gets rid of a warning on DCERouter startup

mkdir -p /usr/pluto/bin/plugin-inject
LogFile="/var/log/pluto/DCERouter.log";

echo "== FRESH START =="
Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Writing Version: $Version"
Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Writing Version: $Version" "$LogFile"
echo "$Version" >/home/pluto-version

cd /usr/pluto/bin
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter"
Logging "$TYPE" "$SEVERITY_NORMAL" "$0" "Starting DCERouter" "$LogFile"

module=DCERouter
device_name="$module"

MAX_LOOP_COUNT=200

i=1
while [[ "$i" -le "$MAX_LOOP_COUNT" ]]; do
	Log "$LogFile" "$LogSectionDelimiter"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i" "$LogFile"
	Log "$LogFile" "$(date) Starting"

	/usr/pluto/bin/UpdateEntArea $PLUTO_DB_CRED -D "$MySqlDBName" > >(tee -a /var/log/pluto/updateea.log)
	mysqldump $MYSQL_DB_CRED "$MySqlDBName" Device > /var/log/pluto/device.$(date +%F-%T)
	if [[ "${Valgrind/DCERouter}" != "$Valgrind" ]]; then
		/usr/pluto/bin/Spawn_Wrapper.sh $VGcmd/usr/pluto/bin/DCERouter $PLUTO_DB_CRED -D "$MySqlDBName" -l "$LogFile"
	else
		/usr/pluto/bin/Spawn_Wrapper.sh /usr/pluto/bin/DCERouter $PLUTO_DB_CRED -D "$MySqlDBName" -l "$LogFile"
	fi

	Ret="$?"
	Log "$LogFile" "Return code: $Ret"
	if [[ "$Ret" -eq 0 ]]; then
		# Abort
		Logging $TYPE $SEVERITY_NORMAL "$module" "Shutting down... count=$i/$MAX_LOOP_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_NORMAL "$module" "Shutting down... count=$i/$MAX_LOOP_COUNT dev=$device_name" "$LogFile"
		Log "$LogFile" "$(date) Shutdown"
		exit
	elif [[ "$Ret" -eq 2 ]]; then
		Logging $TYPE $SEVERITY_NORMAL "$module" "Device requests restart... count=$i/$MAX_LOOP_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_NORMAL "$module" "Device requests restart... count=$i/$MAX_LOOP_COUNT dev=$device_name" "$LogFile"
		Log "$LogFile" "$(date) Restart"
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... count=$i/$MAX_LOOP_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... count=$i/$MAX_LOOP_COUNT dev=$device_name" "$LogFile"
		Log "$LogFile" "$(date) died"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/died_${device_id}_$device_name
		sleep 5
		((i++))
	fi
	echo out
done

