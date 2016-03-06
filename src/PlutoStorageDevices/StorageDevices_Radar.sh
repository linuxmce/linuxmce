#!/bin/bash
. /usr/pluto/bin/SQL_Ops.sh

# Make sure we run with US numbers and stuff.
LC_ALL=C

#DEBUG="yes"

export PATH="$PATH:/sbin:/usr/sbin:/usr/local/sbin"
availPath=""
mountedDevPath=""
mountedPath=""
Hdds=""
Hdd_DT="DT:1790"
DD_Uuid="267"
shopt -u nullglob

## Function to remove undesired paths
diff_Funk(){
	awk 'BEGIN{RS=ORS=" "}
	{NR==FNR?a[$0]++:a[$0]--}
	END{for(k in a)if(a[k])print k}' <(echo -n "${!1}") <(echo -n "${!2}")
}

Detect() {

	## Available Device Paths
	for id in $(blkid -o device ) ; do
		Drive=$(readlink -f "${id}")
		# Add Drive if it is not null and not already in Hdds
		if [[ -n "$Drive" ]] && [[ "$Hdds" != *"$Drive"* ]] ; then
			Hdds=$(echo "$Hdds $Drive" | sed -e 's/^[ \t]*//')
		elif [[ -z "$Drive" ]] ; then
			Hdds=$(echo "$Hdds $id" | sed -e 's/^[ \t]*//')
		fi
	done
	availPath="$Hdds"
	[[ -n "$DEBUG" ]] && echo "availPath: $availPath" # DEBUG

	## Looks for standard device names mounted in mtab and learns path. Note some mounted drives show only UUID
	mountedDevName=$(cat /etc/mtab | awk '/dev\/(sd|hd|md|xd|vd|mmc)./ {print $1}')
	for Drive in $mountedDevName ; do
		mountedDevGrep=$(blkid ${Drive} | cut -d: -f1)
		mountedDevPath=$(echo "$mountedDevPath $mountedDevGrep" | sed -e 's/^[ \t]*//')
	done
	[[ -n "$DEBUG" ]] && echo "mountedDevPath: $mountedDevPath" # DEBUG

	## Grabs UUID of mounted devices in mtab and learns path. Note some mounted drives only show device names.
	mountedUuid=$(cat /etc/mtab | awk '/uuid/ { print $1 }' | sort -u)
	for id in $mountedUuid ; do
		Drive=$(readlink -f "${id}")
		# Add dev link name if it is not null and not already in mounted path
		# Add the full uuid path if no link is found
		if [[ -n "$Drive" ]] && [[ "$mountedPath" != *"$Drive"* ]] ; then
			mountedPath=$(echo "$mountedPath $Drive" | sed -e 's/^[ \t]*//')
		elif [[ -z "$Drive" ]] ; then
			mountedPathFind=$(blkid $(basename ${id}) | cut -d: -f1)
			mountedPath=$(echo "$mountedPath $mountedPathFind" | sed -e 's/^[ \t]*//')
		fi
	done
	[[ -n "$DEBUG" ]] && echo "mountedPath: $mountedPath" # DEBUG

	## Find root entry on command line
	cmdlineDevName=""
	set -- $(cat /proc/cmdline)
	for x in "$@"; do
	    case "$x" in
	        root=*)
	        cmdlineDevName="${x#root=}"
	        ;;
	    esac
	done

	## Looks for standard device names mounted on /proc/cmdline like Raspberry Pi has.
	cmdlineDevAwk=$(echo "$cmdlineDevName" | awk '/dev\/(sd|hd|md|xd|vd|mmc)./ {print $1}')
	for Drive in $cmdlineDevAwk ; do
		mountedDevGrep=$(blkid ${Drive} | cut -d: -f1)
		mountedDevPath=$(echo "$mountedDevPath $mountedDevGrep" | sed -e 's/^[ \t]*//')
	done
	[[ -n "$DEBUG" ]] && echo "mountedDevPath: $mountedDevPath" # DEBUG

	## Grabs UUID of mounted devices in mtab and learns path. Note some mounted drives only show device names.
	cmdlineUuid=$(echo "$cmdlineDevName" | awk '/UUID/ { print $1 }' | cut -d'=' -f2)
	for Drive in $cmdlineUuid ; do
		mountedPathFind=$(blkid ${Drive} | cut -d: -f1)
		mountedPath=$(echo "$mountedPath $mountedPathFind" | sed -e 's/^[ \t]*//')
	done
	[[ -n "$DEBUG" ]] && echo "mountedPath: $mountedPath" # DEBUG

	## Finds UUID of devices that have been told to be ignored.
	Q="
		SELECT
		SerialNumber
		FROM
		UnknownDevices
		WHERE
		FK_Device_PC = '$PK_Device' AND VendorModelId = '$Hdd_DT'
	"
	uDrives=$(RunSQL "$Q")
	for Drive in $uDrives ; do
		unknownPathFind=$(blkid ${Drive} | cut -d: -f1)
		unknownPath=$(echo "$unknownPath $unknownPathFind" | sed -e 's/^[ \t]*//')
	done
	[[ -n "$DEBUG" ]] && echo "unknownPath: $unknownPath" # DEBUG

	## Remove the mounted paths or unavailable paths
	subtractPath=$(echo "$unknownPath $mountedPath $mountedDevPath")
	[[ -n "$DEBUG" ]] && echo "subtractPath: $subtractPath" # DEBUG
	availPath=($(diff_Funk availPath[@] subtractPath[@]))
	[[ -n "$DEBUG" ]] && echo "diff_Funk: $availPath" # DEBUG
	availPath=$(echo "${availPath[@]}")
	[[ -n "$DEBUG" ]] && echo "availPath: $availPath" # DEBUG

## TODO: reimplement this?  is this necessary with blkid?
#	# check for aliases to see if this is already mounted under another path/name
#	for Path in $availPath; do
#		for path_alias in $(udevadm info --query=symlink --name="/dev/disk/by-path/$Path" | awk '{print $1}'); do
#			new_path_alias=$(udevadm info --query=all --name=/dev/${path_alias} | grep 'S: disk/by-path/' | awk -F/ '{print $NF}' | sort -u)
#			alias_mounted=$(mount | grep 'dev/disk' | awk '{print $1}')
#			alias_mounted_path=$(udevadm info --query=all --name=${alias_mounted} | grep 'S: disk/by-path/' | awk -F/ '{print $NF}' | sort -u)
#			for each_nam in $alias_mounted_path; do
#				if [[ "$new_path_alias" == "$each_nam" ]]; then
#					availPath=($(diff_Funk availPath[@] Path[@]))
#					availPath=$(echo ${availPath[@]})
#				fi
#			done
#		done
#	done

	auxPath=""
	for Path in $availPath ; do
		## If is extended partition
		if file -sL $Path | grep -q "extended partition table" ; then
			continue
		fi
		## If is swap partition
		if file -sL $Path | grep -q "swap file" ; then
			continue
		fi
		## If is boot sector
		if file -sL $Path | grep -q "boot sector;" ; then
			continue
		fi

		auxPath=$(echo "$auxPath $Path" | sed -e 's/^[ \t]*//')
	done
	availPath=$auxPath
	[[ -n "$DEBUG" ]] && echo "auxPath: $auxPath" # DEBUG

	## Remove paths that belong to a mounted RAID
	if [[ -x /sbin/mdadm ]]; then
		auxPath=""
		for Path in $availPath; do
			if [[ "$(mdadm --examine ${Path} 2>&1)" == *"No md superblock"* ]]; then
				auxPath=$(echo "$auxPath $Path" | sed -e 's/^[ \t]*//')
			fi
		done
		availPath="$auxPath"
	fi
	[[ -n "$DEBUG" ]] && echo "availPath: $availPath" # DEBUG

	## Test to see if we found any available paths
	if [[ "$availPath" != "" ]]; then
		for pathPosition in $availPath; do
			## I assume someone left this non working MessageSend line in for reference, and have done the same.
			#/usr/pluto/bin/MessageSend $DCERouter 0 $OrbiterIDList 1 741 159 228 109 "[$pathPosition]" 156 $PK_Device 163 "$InfoMessage"

			## Get info about this volume
			partition_diskname=$(udevadm info --query=all --name="${pathPosition}" | grep 'ID_MODEL=' | cut -d'=' -f2)
			partition_serial=$(udevadm info --query=all --name="${pathPosition}" | grep 'ID_SERIAL_SHORT=' | cut -d'=' -f2)
			partition_label=$(udevadm info --query=all --name="${pathPosition}" | grep 'ID_FS_LABEL=' | cut -d'=' -f2)
			partition_uuid=$(udevadm info --query=all --name="${pathPosition}" | grep 'ID_FS_UUID=' | cut -d'=' -f2)
			partition=$(udevadm info --query=all --name="${pathPosition}" | grep 'N:' | awk '{ print $2 }')
			partition_size=$(expr $(udevadm info -a -n "/dev/${partition}" | grep 'ATTR{size}' | sed 's/.*=//;s/"//g') / 2 )

			## fallback to serial number if no UUID available (for instance FAT devices)
			if [[ "$partition_uuid" == "" && "$partition_serial" != "" ]]; then
				partition_uuid="$partition_serial"
			fi

			## fallback to volume label if no UUID or serial number available
			if [[ "$partition_uuid" == "" && "$partition_label" != "" ]]; then
				partition_uuid="$partition_label"
			fi

			if [[ "$partition_uuid" == "" ]]; then
				continue
			fi

			## make sure drive is bigger than 0
			if [[ "$partition_size" == "0" ]]; then
				continue
			fi

			## Convert size to human readable
			num_size=$(echo -n "$partition_size" | wc -m)

			if [[ "$num_size" -lt "19" ]]; then
				unit="EB"; divisor="1125899906842624"
			fi

			if [[ "$num_size" -lt "16" ]]; then
				unit="PB"; divisor="1099511627776"
			fi

			if [[ "$num_size" -lt "13" ]]; then
				unit="TB"; divisor="1073741824"
			fi

			if [[ "$num_size" -lt "10" ]]; then
				unit="GB"; divisor="1048576"
			fi

			if [[ "$num_size" -lt "7" ]]; then
				unit="MB"; divisor="1024"
			fi

			kb=$(printf '%.2f' $(echo "$partition_size / $divisor" | bc -l))
			partition_size="$kb $unit"

			## Sends data to MessageSend
			thisHost=$(hostname)
			Sent="false"
			Count=0
			while [[ "$Sent" == "false" ]]; do
				/usr/pluto/bin/MessageSend "$DCERouter" "$PK_Device" -1001 2 65 55 "${DD_Uuid}|${partition_uuid}|277|${partition_size}" 54 "$partition_uuid" 52 8 49 1790 13 "$partition_size [${partition}] $partition_diskname on $thisHost"
				err=$?

				if [[ "$err" == "0" ]]; then
					Sent="true"
				else
					Count=$(( Count + 1 ))
					sleep 2
				fi

				if [[ "$Count" == "10" ]]; then
					Sent="true"
				fi
			done
		done
	fi
}
Detect
