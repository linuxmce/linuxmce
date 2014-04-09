#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func 

function printHelp()
{
	Logging "$TYPE" "$SEVERITY_NORMAL" "Invalid paramters: ./Start_LocalDevices.sh [ -d DeviceID [-r RouterAddress ] ]" >&2
}

#lshwd is obsolete and unmaintained in 5 years.  removing to see if anything breaks - phenigma - 2014-04-09
#lshwd # needed for some systems which won't report usb devices until you run lshwd first (bug in usb architecture)
/usr/pluto/bin/UpdateAvailableSerialPorts.sh
/usr/pluto/bin/UpdateAvailableSoundCards.sh
bash -x /usr/pluto/bin/SoundCards_Setup.sh | tee-pluto /var/log/pluto/SoundCards_Setup.log
bash -x /usr/pluto/bin/SoundCards_Setup_ConferenceMicrophones.sh | tee-pluto /var/log/pluto/Soundcards_Setup.log

[[ $# -ne 4 && $# -ne 2 && $# -ne 0 ]] && printHelp && exit
CurrentDevice=$PK_Device

export DISPLAY=:$Display

[[ $# -eq 2 || $# -eq 4 && "$1" != "-d" ]] && printHelp && exit
[[ $# -eq 2 || $# -eq 4 ]] && CurrentDevice="$2"

[[ $# -eq 4 && "$3" != "-r" ]] && printHelp && exit
[[ $# -eq 4 ]] && DCERouter=$4

Logging "$TYPE" "$SEVERITY_NORMAL" "Launching child devices for device: $CurrentDevice"

GhostHuntQ="SELECT *
    FROM Device
    JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate
    LEFT JOIN Device AS Device_Parent on Device.FK_Device_ControlledVia=Device_Parent.PK_Device
    LEFT JOIN DeviceTemplate AS DeviceTemplate_Parent ON Device_Parent.FK_DeviceTemplate=DeviceTemplate_Parent.PK_DeviceTemplate
    WHERE (Device.FK_Device_ControlledVia=1
    OR (Device_Parent.FK_Device_ControlledVia=1 AND DeviceTemplate_Parent.FK_DeviceCategory IN (6,7,8) ) )
    AND DeviceTemplate.FK_DeviceCategory <> 1
"

{
	date -R
	RunSQL "$GhostHuntQ"
} >>/var/log/pluto/start_devices.log

Q="SELECT Device.PK_Device, DeviceTemplate.Description, DeviceTemplate.CommandLine, Device.Disabled
	FROM Device
	JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate
	LEFT JOIN Device AS Device_Parent on Device.FK_Device_ControlledVia=Device_Parent.PK_Device
	LEFT JOIN DeviceTemplate AS DeviceTemplate_Parent ON Device_Parent.FK_DeviceTemplate=DeviceTemplate_Parent.PK_DeviceTemplate
	WHERE (Device.FK_Device_ControlledVia=$CurrentDevice 
	OR (Device_Parent.FK_Device_ControlledVia=$CurrentDevice AND DeviceTemplate_Parent.FK_DeviceCategory IN (6,7,8) ) )
	AND DeviceTemplate.FK_DeviceCategory <> 1"
CommandList=$(RunSQL "$Q")


PerlCommand="
	chomp;
	\$desc = \$_;
	\$desc =~ s/[ :+=()<]/_/g;
	\$desc =~ s/[->*?\\$\.%\\/]//g;
	\$desc =~ s/#/Num/g;
	\$desc =~ s/__/_/g;
	\$desc =~ s/^_*//g;
	\$desc =~ s/_*\$//g;
	print \"\$desc\n\";
"

AlreadyRunning="/usr/pluto/locks/pluto_spawned_local_devices.txt"
basename=$(basename "$0")
WaitLock "Spawn_Device" "Start_LocalDevices_$CurrentDevice" >>/var/log/pluto/Spawn_Device.log
Logging "$TYPE" "$SEVERITY_WARNING" "$basename" "Start_LocalDevices $*; CommandList: $CommandList"
for command in $CommandList; do
	ChildDeviceID=$(Field 1 "$command")
	ChildDescription=$(Field 2 "$command" | perl -n -e "$PerlCommand")
	ChildCommand=$(Field 3 "$command")
	ChildIgnore=$(Field 4 "$command")

	if [[ "$ChildIgnore" -eq 1 ]]; then
		Logging "$TYPE" "$SEVERITY_WARNING" "$basename" "Child device ($ChildDeviceID ; $ChildDescription) disabled"
		continue
	fi

	Logging "$TYPE" "$SEVERITY_NORMAL" "$basename" "$$ Dev: $ChildDeviceID; Already Running list: $(cat $AlreadyRunning | tr '\n' ',')"
	if grep -q "^$ChildDeviceID$" "$AlreadyRunning" 2>/dev/null; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$basename" "$$ Device $ChildDeviceID was marked as 'running'. Not starting"
		echo "$(date) SLD Already running: $ChildDeviceID" >>/var/log/pluto/Spawn_Device.log
		continue
	fi

	if [[ -z "$ChildCommand" ]]; then
		ChildCommand="$ChildDescription"
	fi

	if [[ ! -f "$Path/usr/pluto/bin/$ChildCommand" ]]; then
		Logging "$TYPE" "$SEVERITY_WARNING" "$basename" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) is not available in /usr/pluto/bin."
	elif [[ ! -x "$Path/usr/pluto/bin/$ChildCommand" ]]; then
		Logging "$TYPE" "$SEVERITY_WARNING" "$basename" "Child device ($ChildDeviceID) was configured but the startup script ($ChildCommand) existent in /usr/pluto/bin is not executable."
	elif [[ "$basename" = "$(basename $ChildCommand)" ]]; then
		pushd /usr/pluto/bin >/dev/null
		screen -d -m -S "${ChildDescription}_SLD-$ChildDeviceID" /usr/pluto/bin/$ChildCommand -d $ChildDeviceID -r $DCERouter
		popd >/dev/null
	else
		Logging "$TYPE" "$SEVERITY_NORMAL" "$basename" "Launching device $ChildDeviceID in screen session ($ChildDescription)"
		pushd /usr/pluto/bin >/dev/null
		screen -d -m -S "$ChildDescription-$ChildDeviceID" /usr/pluto/bin/Spawn_Device.sh $ChildDeviceID $DCERouter $ChildCommand
		popd >/dev/null
	fi
done

Q="SELECT PK_Device FROM Device WHERE Disabled=0"
EnabledDevices=" $(RunSQL "$Q") "
RunningDevices="$(< "$AlreadyRunning")"
for Device in $RunningDevices; do
	if [[ "$EnabledDevices" != *" $Device "* ]]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "$basename" "Telling device '$Device' to stop"
		/usr/pluto/bin/MessageSend "$DCERouter" 0 "$Device" 7 0 163 "start_local_devices"
	fi
done

Unlock "Spawn_Device" "Start_LocalDevices_$CurrentDevice" >>/var/log/pluto/Spawn_Device.log
