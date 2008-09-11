#!/bin/bash
#<-mkr_b_ubuntu_b->

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Utils.sh

MAX_RESPAWN_COUNT=50

mkdir -p /var/log/pluto
echo "$(date) Spawn_Device $*" >> /var/log/pluto/Spawn_Device.log
# syntax: $0 <device_id> <ip_of_router> <cmd_line> 

. /usr/pluto/bin/pluto.func || exit

TYPE=spawning-device
device_id="$1"
ip_of_router="$2"
device_name="$device_id";
module="$device_name" #$(basename $0)
cmd_line="$3"

LogFile="/var/log/pluto/${device_id}_$(basename $cmd_line).log";
valgrind_LogFile="/var/log/pluto/valgrind_${device_id}_$(basename $cmd_line).log";

UseAlternativeLibs

Log "$LogFile" "== ATTEMPT FRESH START =="
AlreadyRunning="/usr/pluto/locks/pluto_spawned_local_devices.txt"
AlreadyRunningPids="$(cat "$AlreadyRunning" | tr '\n' ',')"
WaitLock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Already Running list: $AlreadyRunningPids"
Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Already Running list: $AlreadyRunningPids" "$LogFile"
if grep -q "^$device_id$" "$AlreadyRunning" 2>/dev/null; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Device $device_id was marked as 'running'. Not starting"
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Device $device_id was marked as 'running'. Not starting" "$LogFile"
	echo "$(date) Already running: $device_id" >>/var/log/pluto/Spawn_Device.log
	Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Exiting because not starting"
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Exiting because not starting" "$LogFile"
	exit
fi
Log "$LogFile" "== FRESH START =="
echo "$device_id" >>"$AlreadyRunning"
echo "$(date) About to be run: $device_id" >>/var/log/pluto/Spawn_Device.log
Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log

Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "device: $device_id ip: $ip_of_router cmd_line: $cmd_line"
Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "device: $device_id ip: $ip_of_router cmd_line: $cmd_line" "$LogFile"

[[ -n "$VGcmd" ]] && VGcmd="$VGcmd --log-file=$valgrind_LogFile "

if [[ -z "$cmd_line" ]]; then
	Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Empty command line"
	Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Empty command line" "$LogFile"
	exit 1
fi

OK=0
if [[ -x "./$cmd_line" ]]; then
	Path="./"
	OK=1
elif [[ -x "/usr/pluto/bin/$cmd_line" ]]; then
	Path="/usr/pluto/bin/"
	cd $Path
	OK=1
elif [[ -x "$cmd_line" ]]; then
	Path=
	OK=1
else
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0 $module" "Can't execute $cmd_line"
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0 $module" "Can't execute $cmd_line" "$LogFile"
	sed -i "/^$device_id$/ d" "$AlreadyRunning"
	exit 1
fi

if [[ -z "$display" ]]; then
	display=":0"
fi

[[ -z "$DISPLAY" ]] && export DISPLAY=:$Display

Logging $TYPE $SEVERITY_STAGE "$module" "Entering $module"
Logging $TYPE $SEVERITY_STAGE "$module" "Entering $module" "$LogFile"

# wait for the package to be installed if this is the case
while [[ -f /usr/pluto/locks/installing."$device_id" ]]; do
	sleep 1
done

echo "$$ Spawn_Device of $Path$cmd_line (by $0)" >>/var/log/pluto/running.pids

Tab=$'\t' # to prevent any smart masses from converting this tab to 4 or 8 spaces
i=1
ReloadWatcher=0
while [[ "$i" -le "$MAX_RESPAWN_COUNT" ]]; do
	Log "$LogFile" "$LogSectionDelimiter"
	rm -f /var/log/pluto/spawned_devices_$device_id

	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i" "$LogFile"

	Logging $TYPE $SEVERITY_NORMAL "$module" "Found $Path$cmd_line"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Found $Path$cmd_line" "$LogFile"
	if [[ "$cmd_line" != *Spawn_Device* && "$Valgrind" == *"$cmd_line"* ]]; then
#		(/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id")
		/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router" -l "$LogFile"
	else
#		(/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id")
		/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router" -l "$LogFile"
	fi

#	Ret="$(cat /tmp/pluto_exitcode_$device_id)"
	Ret="$?"
	rm -f /tmp/pluto_exitcode_$device_id
	
#	if [ -f /var/log/pluto/spawned_devices_$device_id ]; then
#		while read line; do
#			screen -list | grep -F "$line" | cut -d. -f1 | cut -d"$Tab" -f2 | xargs kill -9
#		done < /var/log/pluto/spawned_devices_$device_id
#	fi
#	screen -wipe &>/dev/null
	
	Log "$LogFile" "Return code: $Ret"
	if [[ "$Ret" -eq 0 ]]; then
		# Abort
		WaitLock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
		sed -i "/^[^0-9]*$device_id[^0-9]*\$/ d" "$AlreadyRunning"
		Logging $TYPE $SEVERITY_WARNING "$module" "Shutting down... count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_WARNING "$module" "Shutting down... count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		sed -i "/^$device_id$/ d" "$AlreadyRunning"
		Log "$LogFile" "$(date) Shutdown"
		Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
		break
	elif [[ "$Ret" -eq 2 ]]; then
		Logging $TYPE $SEVERITY_WARNING "$module" "Device requests restart... count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_WARNING "$module" "Device requests restart... count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		if DeviceIsDisabled "$device_id"; then
			WaitLock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
			Logging $TYPE $SEVERITY_WARNING "$module" "Device was disabled or removed. Stopping and marking as not running."
			Logging $TYPE $SEVERITY_WARNING "$module" "Device was disabled or removed. Stopping and marking as not running." "$LogFile"
			sed -i "/^$device_id$/ d" "$AlreadyRunning"
			Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
			break
		fi
		Log "$LogFile" "$(date) Restart"
		sleep 8
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		Log "$LogFile" "$(date) died"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/died_${device_id}_$device_name

		count=0
		echo /usr/pluto/coredump/core_${cmd_line##*/}_* | tr ' ' '\n' | while read line; do
			((count++))
			if [[ "$count" -gt 3 ]]; then
				rm -f $line
			fi
		done
		if [[ "$count" -gt 3 ]]; then
			Logging $TYPE $SEVERITY_WARNING "$module" "Found $count cores. Deleted all but latest 3. count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
			Logging $TYPE $SEVERITY_WARNING "$module" "Found $count cores. Deleted all but latest 3. count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		fi
		sleep 10
		((i++))
	fi
	echo out
done
if [[ "$i" -gt "$MAX_RESPAWN_COUNT"  ]]; then
	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device $device_id..."
	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device $device_id..." "$LogFile"
fi
#<-mkr_b_ubuntu_e->
#<-mkr_B_ubuntu_b->

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Utils.sh

MAX_RESPAWN_COUNT=50

mkdir -p /var/log/pluto
echo "$(date) Spawn_Device $*" >> /var/log/pluto/Spawn_Device.log
# syntax: $0 <device_id> <ip_of_router> <cmd_line> 

. /usr/pluto/bin/pluto.func || exit

TYPE=spawning-device
device_id="$1"
ip_of_router="$2"
device_name="$device_id";
module="$device_name" #$(basename $0)
cmd_line="$3"

LogFile="/var/log/pluto/${device_id}_$(basename $cmd_line).log";
valgrind_LogFile="/var/log/pluto/valgrind_${device_id}_$(basename $cmd_line).log";

ReloadWatcher_Background()
{
	local InitCmd
	InitCmd=$(cat /proc/1/cmdline | tr '\0' ' ')
	if [[ "$InitCmd" != *2* ]]; then
		return
	fi
	
	/usr/pluto/bin/Config_Device_Changes.sh
	/usr/pluto/bin/Start_LocalDevices.sh
}

UseAlternativeLibs

Log "$LogFile" "== ATTEMPT FRESH START =="
AlreadyRunning="/usr/pluto/locks/pluto_spawned_local_devices.txt"
AlreadyRunningPids="$(cat "$AlreadyRunning" | tr '\n' ',')"
WaitLock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Already Running list: $AlreadyRunningPids"
Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Already Running list: $AlreadyRunningPids" "$LogFile"
if grep -q "^$device_id$" "$AlreadyRunning" 2>/dev/null; then
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Device $device_id was marked as 'running'. Not starting"
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Device $device_id was marked as 'running'. Not starting" "$LogFile"
	echo "$(date) Already running: $device_id" >>/var/log/pluto/Spawn_Device.log
	Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Exiting because not starting"
	Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "$$ Dev: $device_id; Exiting because not starting" "$LogFile"
	exit
fi
Log "$LogFile" "== FRESH START =="
echo "$device_id" >>"$AlreadyRunning"
echo "$(date) About to be run: $device_id" >>/var/log/pluto/Spawn_Device.log
Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log

Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "device: $device_id ip: $ip_of_router cmd_line: $cmd_line"
Logging "$TYPE" "$SEVERITY_NORMAL" "$0 $module" "device: $device_id ip: $ip_of_router cmd_line: $cmd_line" "$LogFile"

[[ -n "$VGcmd" ]] && VGcmd="$VGcmd --log-file=$valgrind_LogFile "

if [[ -z "$cmd_line" ]]; then
	Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Empty command line"
	Logging "$TYPE" "$SEVERITY_WARNING" "$0 $module" "Empty command line" "$LogFile"
	exit 1
fi

OK=0
if [[ -x "./$cmd_line" ]]; then
	Path="./"
	OK=1
elif [[ -x "/usr/pluto/bin/$cmd_line" ]]; then
	Path="/usr/pluto/bin/"
	cd $Path
	OK=1
elif [[ -x "$cmd_line" ]]; then
	Path=
	OK=1
else
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0 $module" "Can't execute $cmd_line"
	Logging "$TYPE" "$SEVERITY_CRITICAL" "$0 $module" "Can't execute $cmd_line" "$LogFile"
	sed -i "/^$device_id$/ d" "$AlreadyRunning"
	exit 1
fi

if [[ -z "$display" ]]; then
	display=":0"
fi

[[ -z "$DISPLAY" ]] && export DISPLAY=:$Display

Logging $TYPE $SEVERITY_STAGE "$module" "Entering $module"
Logging $TYPE $SEVERITY_STAGE "$module" "Entering $module" "$LogFile"

# wait for the package to be installed if this is the case
while [[ -f /usr/pluto/locks/installing."$device_id" ]]; do
	sleep 1
done

echo "$$ Spawn_Device of $Path$cmd_line (by $0)" >>/var/log/pluto/running.pids

Tab=$(printf "\t") # to prevent any smart masses from converting this tab to 4 or 8 spaces
i=1
ReloadWatcher=0
while [[ "$i" -le "$MAX_RESPAWN_COUNT" ]]; do
	Log "$LogFile" "$LogSectionDelimiter"
	rm -f /var/log/pluto/spawned_devices_$device_id

	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Starting... $i" "$LogFile"

	ReloadLock=/usr/pluto/locks/reload_watcher
	if [[ -f "$ReloadLock" ]] && grep -q "$device_id" "$ReloadLock"; then
		if [[ "$ReloadWatcher" -eq 0 ]]; then
			Logging $TYPE $SEVERITY_WARNING "$module" 'Reload watcher: I am the one, but did not know it! Assuming position'
			Logging $TYPE $SEVERITY_WARNING "$module" 'Reload watcher: I am the one, but did not know it! Assuming position' "$LogFile"
		fi
		ReloadWatcher=1
	fi

	if [[ "$ReloadWatcher" -eq 1 ]]; then
		Logging $TYPE $SEVERITY_WARNING "$module" "Reload watcher: running Start_LocalDevices"
		Logging $TYPE $SEVERITY_WARNING "$module" "Reload watcher: running Start_LocalDevices" "$LogFile"
		/usr/pluto/bin/UpdateAvailableSerialPorts.sh
		ReloadWatcher_Background &
		disown -a
	fi

	if [[ "$cmd_line" == *App*Server* && ! -f "$ReloadLock" ]]; then
		if Lock "Reload_Watcher" "$device_id"; then
			echo "Device: $device_id" >"$ReloadLock"
			ReloadWatcher=1
			Logging $TYPE $SEVERITY_WARNING "$module" "Reload watcher: I, $device_id, have just been assigned as the reload watcher"
			Logging $TYPE $SEVERITY_WARNING "$module" "Reload watcher: I, $device_id, have just been assigned as the reload watcher" "$LogFile"
			# We don't unlock this one, ever
		fi
	fi
	Logging $TYPE $SEVERITY_NORMAL "$module" "Found $Path$cmd_line"
	Logging $TYPE $SEVERITY_NORMAL "$module" "Found $Path$cmd_line" "$LogFile"
	if [[ "$cmd_line" != *Spawn_Device* && "$Valgrind" == *"$cmd_line"* ]]; then
#		(/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id")
		/usr/pluto/bin/Spawn_Wrapper.sh $(echo "$VGcmd")"$Path$cmd_line" -d "$device_id" -r "$ip_of_router" -l "$LogFile"
	else
#		(/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router"; echo $? >"/tmp/pluto_exitcode_$device_id")
		/usr/pluto/bin/Spawn_Wrapper.sh "$Path$cmd_line" -d "$device_id" -r "$ip_of_router" -l "$LogFile"
	fi

#	Ret="$(cat /tmp/pluto_exitcode_$device_id)"
	Ret="$?"
	rm -f /tmp/pluto_exitcode_$device_id
	
#	if [ -f /var/log/pluto/spawned_devices_$device_id ]; then
#		while read line; do
#			screen -list | grep -F "$line" | cut -d. -f1 | cut -d"$Tab" -f2 | xargs kill -9
#		done < /var/log/pluto/spawned_devices_$device_id
#	fi
#	screen -wipe &>/dev/null
	
	Log "$LogFile" "Return code: $Ret"
	if [[ "$Ret" -eq 0 ]]; then
		# Abort
		WaitLock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
		sed -i "/^[^0-9]*$device_id[^0-9]*\$/ d" "$AlreadyRunning"
		Logging $TYPE $SEVERITY_WARNING "$module" "Shutting down... count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_WARNING "$module" "Shutting down... count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		sed -i "/^$device_id$/ d" "$AlreadyRunning"
		Log "$LogFile" "$(date) Shutdown"
		Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
		break
	elif [[ "$Ret" -eq 2 ]]; then
		Logging $TYPE $SEVERITY_WARNING "$module" "Device requests restart... count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_WARNING "$module" "Device requests restart... count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		if DeviceIsDisabled "$device_id"; then
			WaitLock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
			Logging $TYPE $SEVERITY_WARNING "$module" "Device was disabled or removed. Stopping and marking as not running."
			Logging $TYPE $SEVERITY_WARNING "$module" "Device was disabled or removed. Stopping and marking as not running." "$LogFile"
			sed -i "/^$device_id$/ d" "$AlreadyRunning"
			Unlock "Spawn_Device" "$device_id" >>/var/log/pluto/Spawn_Device.log
			break
		fi
		Log "$LogFile" "$(date) Restart"
		sleep 8
	else
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
		Logging $TYPE $SEVERITY_CRITICAL "$module" "Device died... count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		Log "$LogFile" "$(date) died"
		echo $(date) $device_name died >> /var/log/pluto/deaths
		echo $(date) died >> /var/log/pluto/died_${device_id}_$device_name

		count=0
		echo /usr/pluto/coredump/core_${cmd_line##*/}_* | tr ' ' '\n' | while read line; do
			((count++))
			if [[ "$count" -gt 3 ]]; then
				rm -f $line
			fi
		done
		if [[ "$count" -gt 3 ]]; then
			Logging $TYPE $SEVERITY_WARNING "$module" "Found $count cores. Deleted all but latest 3. count=$i/$MAX_RESPAWN_COUNT dev=$device_name"
			Logging $TYPE $SEVERITY_WARNING "$module" "Found $count cores. Deleted all but latest 3. count=$i/$MAX_RESPAWN_COUNT dev=$device_name" "$LogFile"
		fi
		sleep 10
		((i++))
	fi
	echo out
done
if [[ "$i" -gt "$MAX_RESPAWN_COUNT"  ]]; then
	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device $device_id..."
	Logging $TYPE $SEVERITY_CRITICAL $module "Aborting restart of device $device_id..." "$LogFile"
fi
#<-mkr_B_ubuntu_e->
