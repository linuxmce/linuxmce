#!/bin/bash

. /usr/pluto/bin/Section_Ops.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/LockUtils.sh


Computer="$1"
BlockDevice="$2"
Action="$3"

WorkDir="/mnt/optical"

trap "Unlock 'DVDShare-$BlockDevice' 'StorageDevices_DVD.sh'" EXIT
WaitLock "DVDShare-$BlockDevice" 'StorageDevices_DVD.sh'

function Log {
	echo "$(date -R) $$ $*" >&2 
	echo "$(date -R) $$ $*" >> /var/log/pluto/StorageDevices_DVD.log
}

mkdir -p "$WorkDir"
Log "Called $0 $*"

function MountDVD {
	umount -lf "/dev/${BlockDevice}" 2>/dev/null
	
	if ! mount -t auto -o user,noauto "/dev/${BlockDevice}" "$WorkDir/${Computer}_${BlockDevice}" ;then
		Log "Cannot mount /dev/${BlockDevice}"
		exit 1
	fi
}

## If the dvb drive is from this computer
if [[ "$Action" == "start" ]] ;then
	if [[ "$Computer" == "$PK_Device" ]] ;then

		## Test to see if the block device exists
		if [[ ! -b "/dev/${BlockDevice}" ]] ;then
			Log "Can't find block device '/dev/${BlockDevice}'"
			exit 1
		fi

		## Test to see if the block device contains a mountable dvd
		## by checking it's filesystem to be UDF or ISO9660
		if ! file -s -L "/dev/${BlockDevice}" | grep -q "UDF\|ISO" ;then
			Log "Filesystem on '/dev/${BlockDevice}' is not UDF or ISO9660"
			exit 1
		fi
		
		## Mount it where it should be
		DvdChecksum_cur=$(/usr/pluto/bin/dvd_unique_id /dev/$BlockDevice)
		DvdChecksum_old=$(cat $WorkDir/$BlockDevice.checksum)
		mkdir -p "${WorkDir}/${Computer}_${BlockDevice}"
		if [[ "$DvdChecksum_cur" != "$DvdChecksum_old" ]] ;then
			Log "Checksums are different new=$DvdChecksum_cur old=$DvdChecksum_old. Re mounting"
			MountDVD
			echo "$DvdChecksum_cur" > "$WorkDir/$BlockDevice.checksum"
		else
			Log "Checksums are similar, skiped remounting"
		fi

		## Add a samba share for it so it'll be accesible by other computers too
		SambaSection="
		[Optical_${BlockDevice}\$]
		comment = Optical Drive ${BlockDevice}
		browsable = yes
		writable = no
		path = ${WorkDir}/${Computer}_${BlockDevice}
		public = no
		guest ok = no
		force user = root
		force group = root
		"

		PopulateSection "/etc/samba/smb.conf" "Optical_Drive_${BlockDevice}" "$SambaSection"
		if [[ "$(pidof smbd)" == "" ]] ;then
			service smbd restart &>/dev/null
		else
			service smbd reload &>/dev/null
		fi

	## If is from another computer
	else
		## Umount if firs, nobody should use it at this point althought is lazy
		umount -lf "${WorkDir}/${Computer}_${BlockDevice}" 2>/dev/null

		## Get the IP Address of the core
		IPaddress=$(RunSQL "SELECT IPaddress from Device WHERE PK_Device = '$Computer' LIMIT 1")
		if [[ "$IPaddress" == "" ]] ;then
			Log "Cannot find IP address associated with device '$Computer'"
			exit 1
		fi

		## Tell that computer to enable the dvd drive
		if ! ssh root@"$IPaddress" "/usr/pluto/bin/StorageDevices_DVD.sh $1 $2 $3" ;then
			Log "Cannot comunicate with $Computer to enable DVD Drive sharing but continue anyway"
		fi

		mkdir -p "${WorkDir}/${Computer}_${BlockDevice}"

		## Mount / Remount the dvd drive
		if ! mount -t "smbfs" -o "ro,port=139,credentials=/usr/pluto/var/sambaCredentials.secret" "//$IPaddress/Optical_${BlockDevice}\$" "${WorkDir}/${Computer}_${BlockDevice}" ;then
			Log "Cannot mount the remote DVD"
			exit 1
		fi

		Log "Mounted successfull"
	fi

elif [[ "$Action" == "stop" ]] ;then

	if [[ "$Computer" != "$PK_Device" ]] ;then
		umount -lf "${WorkDir}/${Computer}_${BlockDevice}"
	fi
fi
