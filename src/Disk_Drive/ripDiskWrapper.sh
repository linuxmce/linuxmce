#!/bin/bash


. /usr/pluto/bin/Config_Ops.sh

# This is stupid i know but it is all that i could find as a solution in the hurry
#	See Disk_Drive.h for the treated values.
ERR_NOT_SUPPORTED_YET=6
ERR_RESULT_FAILURE=6
ERR_SUCCESS=5


# internal script errors
ERR_NONE=0

function isFullDisk
{
	file="/${1%/*}"
	blocks=$(stat --format=%a -f "$file")
	block_size=$(stat --format=%S -f "$file")
	size=$((blocks * block_size / 1024 / 1024))
	if [[ "$size" -gt 1 ]]; then
		echo '0'
	else
		echo '1'
	fi
}

function isReadOnly
{
	file="$1"
	path=${file%/*}
	touch "$path"/testing 2>/dev/null
	if [[ -e "$path"/testing ]]; then
		rm "$path"/testing
		echo '0'
	else
		echo '1'
	fi
}


function printUsage
{
	echo "$0 <Disk_Drive device id> <job id> <task id> <target file name> <source device> <disk type> <ownerID> <rip format> ['<tracklist>']";
	echo "tracklist format: t1,name1[|t2,name2[|t3,name3]...]"
	echo "  t1 = track number"
	echo "  name1 = file name track is to be saved as"
}

[[ "$#" -lt "6" ]] && printUsage && exit;

for i in "$@"; do
	echo "Parameter: $i"
done

diskDriveDeviceID="$1"
jobID="$2"
taskID="$3"
targetFileName="$4"
sourceDevice="$5"
diskType="$6"
ownerID="$7"
ripFormatString="$8" # mp3, ogg, flac, wav
trackList="$9"

ripFormat=${ripFormatString%%;*}

#Freedb information
FreeDB_URL="http://freedb.freedb.org/~cddb/cddb.cgi"
FreeDB_ProtoVersion=5
FreeDB_User=pluto
FreeDB_Host=$(hostname)
Tab=$'\t'
TrackNumber=0
# end Freedb information

# Disk type possbile values:
#	0 DISCTYPE_CD_AUDIO 
#	1 DISCTYPE_CD
#	2 DISCTYPE_DVD_VIDEO 
#	6 DISCTYPE_CD_MIXED 
#	7 DISCTYPE_CD_VCD
#	8 DISCTYPE_CD_SVCD
#	9 DISCTYPE_DVD_AUDIO
#	10 DISCTYPE_HDDVD
#	11 DISCTYPE_BD

echo "Ripping $sourceDevice to \"$targetFileName\" with a disk of type $diskType for user $ownerID";

command="";
result=$ERR_NONE;
case $diskType in 
	2)
		ProgressOutput='>(/usr/pluto/bin/DiskCopy_ProgressExtract.sh | /usr/pluto/bin/Pluto_Progress.sh "$diskDriveDeviceID" "$jobID" "$taskID")'
		command='/usr/pluto/bin/disc_unlock "$sourceDevice"; nice -n 15 /usr/pluto/bin/disk_copy "$sourceDevice" "$targetFileName.dvd.in-progress" > '"$ProgressOutput"
	;;
	0|1|6|7|8)
		export HTTPGETOPTS="--connect-timeout=5 --tries=1 -q -O -"
		DiscID="$(/usr/bin/cd-discid "$sourceDevice")"		
		if ! Query="$(/usr/bin/cddb-tool query "$FreeDB_URL" "$FreeDB_ProtoVersion" "$FreeDB_User" "$FreeDB_Host" "$DiscID")"; then
			Err=$Err_Query
			echo "Error in query"
		else
			echo "Query Successful"
			Code=$(echo "$Query" | head -1 | cut -d' ' -f1)
			case "$Code" in
				200) # one match
					QueryID=$(echo "$Query" | cut -d' ' -f2,3)
					echo "Query ID code 200, QueryID = $QueryID" > /tmp/QueryID
				;;
				202|403|409|503) # no match/error
					Err=$Err_NoMatch
				;;
				210|211) # multiple match (210 - exact, 211 - inexact)
					QueryID=$(echo "$Query" | head -2 | tail -1 | cut -d' ' -f1,2)
				;;
			esac
		fi

		/usr/bin/cddb-tool read "$FreeDB_URL" "$FreeDB_ProtoVersion" "$FreeDB_User" "$FreeDB_Host" "$QueryID" > /tmp/cddbread.$$
		Read="$(/usr/bin/cddb-tool parse /tmp/cddbread.$$)"
		eval "$Read"
		rm -f /tmp/cddbread.$$
		Tag="$DiscID$Tab$DARTIST$Tab$DALBUM$Tab$CDGENRE$Tab$CDYEAR"
		unset HTTPGETOPTS
		
		Dir="$targetFileName"
		case "$ripFormat" in
			wav)
				FinalExt="wav"
				OutputFile='"$Dir/$FileName.'"$FinalExt"'.in-progress"' # bare file
			;;
			
			ogg)
				FinalExt="ogg"
				Quality="-q $(echo "$ripFormatString" | cut -d';' -f2)"
				OutputFile='>(oggenc -Q --artist "$DARTIST" --album "$DALBUM" --date "$CDYEAR" --genre "$CDGENRE" --tracknum "$TrackNumber" '"$Quality"' -o "$Dir/$FileName.'"$FinalExt"'.in-progress" -)' # encoder
			;;
			
			flac)
				FinalExt="flac"
				OutputFile='>(flac -T "Title=$FileName" -T "Artist=$DARTIST" -T "Album=$DALBUM" -T "TrackNumber=$TrackNumber" -T "Year=$CDYEAR" -T "Genre=$CDGENRE" -o "$Dir/$FileName.'"$FinalExt"'.in-progress" -)' # encoder

			;;
			
			mp3)
				FinalExt="mp3"
				BitRate="-b $(echo "$ripFormatString" | cut -d';' -f2)"
				if [[ "$(echo "$ripFormatString" | cut -d';' -f3)" == "vbr" ]]; then
					BitRate="--vbr-new $BitRate"
				fi
				OutputFile='>(lame -h --tt "$FileName" --ta "$DARTIST" --tl "$DALBUM" --tn "$TrackNumber" --ty "$CDYEAR" --tg "$CDGENRE" '"$BitRate"' - "$Dir/$FileName.'"$FinalExt"'.in-progress")' # encoder
			;;
			
			*)
				echo "Unknown method: $ripFormat"
				exit 1
			;;
		esac
		ProgressOutput='>(/usr/pluto/bin/Paranoia_Progress.sh|/usr/pluto/bin/Pluto_Progress.sh "$diskDriveDeviceID" "$jobID" "$taskID" "$Dir/$FileName")'
		command='nice -n 15 cdparanoia -e -d "$sourceDevice" "$Track" - 2> '"$ProgressOutput"' > '"$OutputFile"
	;;
	10|11)
		# HD-DVD and Blu-ray are ripped into the folder
		RipperBDHD='/usr/pluto/bin/ripBDHD.sh'
		if [ -f "$RipperBDHD" ]; then
			ProgressOutput='/usr/pluto/bin/Pluto_Progress.sh "$diskDriveDeviceID" "$jobID" "$taskID"'
			command='/usr/pluto/bin/ripBDHD.sh "$sourceDevice" "$targetFileName" "$ownerID" | '"$ProgressOutput"
			Dir="$targetFileName"
		else
			command=""
			result=$ERR_NOT_SUPPORTED_YET
		fi
	;;
	*)	result=$ERR_NOT_SUPPORTED_YET;;
esac

echo "Command: $command"
if [[ "$command" == "" ]]; then 
	exit 1;
fi

trap "/usr/pluto/bin/cdop '$sourceDevice' unlock" EXIT # unlock tray on exit
/usr/pluto/bin/cdop "$sourceDevice" lock # lock tray

if [[ "$diskType" == 2 ]]; then
	if eval "$command"; then
		echo "Ripping successful"

		if [ -f "/usr/pluto/bin/dvdcssHelper.sh" ]; then
			echo "Copying also DVD CSS keys"

			export DVDCSS_CACHE="$DVDKeysCache"

			KeysPath=`/usr/pluto/bin/dvdcssHelper.sh -f $sourceDevice | grep "CACHE_DIR:" | sed 's/CACHE_DIR://'`
			if [ -n "$KeysPath" ]; then
				echo "Keys location: $KeysPath"
				KeysDumpStatus=`/usr/pluto/bin/dvdcssHelper.sh -d $sourceDevice | grep "ERROR:"`
				if [ -z "$KeysDumpStatus" ]; then
					# ripped DVD can have different keys cache folder
					RippedKeysPath=`/usr/pluto/bin/dvdcssHelper.sh -f "$targetFileName.dvd.in-progress" | grep "CACHE_DIR:" | sed 's/CACHE_DIR://'`
					echo "Keys location for ripped DVD: $RippedKeysPath"

					KeysFolder=`basename $KeysPath`
					RippedKeysFolder=`basename $RippedKeysPath`
					KeysCacheFolder=`dirname $KeysPath`

					pushd $KeysCacheFolder

					# if this is not the same folder, copy the keys
					if [ "$RippedKeysFolder" = "$KeysFolder" ]; then
						RippedKeysFolder=""
					else
						cp $KeysFolder/* $RippedKeysFolder/
					fi

					tar zcf "$targetFileName.dvd.keys.tar.gz.in-progress" $KeysFolder $RippedKeysFolder
					popd
					mv "$targetFileName.dvd.keys.tar.gz.in-progress" "$targetFileName.dvd.keys.tar.gz"
					echo "Keys copied to $targetFileName.dvd.keys.tar.gz"
				else
					echo "Failed to dump keys"
				fi
			else
				echo "Failed to find destination keys cache dir for the DVD, keys copying aborted"
			fi
		fi

		touch "$targetFileName.dvd.lock"
		mv -f "$targetFileName.dvd"{.in-progress,}
		chmod 664 "$targetFileName.dvd"
		exit 0;
	else
		if [[ "$(isFullDisk "$targetFileName")" == 1 ]]; then
			Message="Disk Full!"
		else
			if [[ "$(isReadOnly "$targetFileName")" == 1 ]]; then
				Message="Disk Read Only!"
			else		
				Message=$(</tmp/rip_message)
			fi
		fi
		echo "Ripping failed: $Message" >> /tmp/riplog
		/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
		rm "$targetFileName.dvd.in-progress";
		exit 1;
	fi
elif [[ "$diskType" == 0 || "$diskType" == 1 || "$diskType" == 6 || "$diskType" == 7 || "$diskType" == 8 ]]; then
	mkdir -p "$Dir"
	if [[ ! -d "$Dir" ]]; then
		$Message="Couldn't create directory";
		/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
		echo "Ripping failed: $Message" >> /tmp/riplog
		exit 1
	fi
	trackList=$(echo "$trackList" | tr ' |' $'\x1 ')
	set -o noglob # prevent shell expansion in for's list
	for File in ${trackList}; do
		set +o noglob
		File=$(echo "$File" | tr $'\x1' ' ')
		Track=${File%%,*}
		FileName=${File#*,}
		FileName=${FileName//\//-}
		TrackNumber=$Track		

		echo "Track: $Track; Filename: $FileName"
		if /usr/pluto/bin/IsDataTrack "$sourceDevice" "$Track"; then
			echo "Skipping track '$Track' because it is a data track on a audio CD"
			continue
		fi
		
		if [[ -z "$FileName" ]]; then
			echo "Skipping track '$Track'. Empty filename"
			continue
		fi

		displaycmd="$command"
		displaycmd="${displaycmd//\\\"/\\\"}"
		echo "Executing: $(eval echo "\"$displaycmd\"")"
		if ! eval "$command"; then
			if [[ "$(isFullDisk "$targetFileName")" == 1 ]]; then
				Message="Disk Full!"
			else
				if [[ "$(isReadOnly "$targetFileName")" == 1 ]]; then
					Message="Disk Read Only!"
				else		
					Message=$(</tmp/rip_message)
				fi
			fi
			
			echo "Ripping failed: $Message" >> /tmp/riplog
			/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
			rm "$Dir/$Filename.$FinalExt.in-progress" &>/dev/null
			exit 1;
		fi
				

		echo "File ripped ok; moving: $Dir/$FileName.$FinalExt.in-progress"
		touch "$Dir/$FileName.$FinalExt.lock"
		mv "$Dir/$FileName.$FinalExt"{.in-progress,}
		chmod 664 "$Dir/$FileName.$FinalExt"
	done
	echo "Ripping successful"
	exit 0;
elif [[ "$diskType" == 10 || "$diskType" == 11 ]]; then
	#mkdir -p "$Dir"
	#if [[ ! -d "$Dir" ]]; then
	#	$Message="Couldn't create directory";
	#	/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
	#	echo "Ripping failed: $Message" >> /tmp/riplog
	#	exit 1
	#fi
	if eval "$command"; then
		echo "Ripping successful"
		exit 0;
	else
		echo "Ripping failed" >> /tmp/riplog
		/usr/pluto/bin/MessageSend "$DCERouter" "$diskDriveDeviceID" "$diskDriveDeviceID" 1 871 258 "$jobID" 257 "$taskID" 199 "e" 9 "$Message" >/dev/null
		exit 1;
	fi
fi

echo "Exiting ripping script"

