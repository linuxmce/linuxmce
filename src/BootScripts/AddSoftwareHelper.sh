#!/bin/bash

. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

Action=$1
DeviceID=$2
PackageID=$3
OrbiterID=$4

ASH_LogFile="/var/log/pluto/AddSoftwareHelper.log"
ASH_Cache="/usr/pluto/var/addsoftware-cache"

MyLog()
{
	Now=`date`
	Log $ASH_LogFile "$Now	[$$]	$1"
	echo $1
}

FindDeviceIP()
{
	Q="SELECT IPAddress FROM Device WHERE PK_Device='$1'"
	Row="$(RunSQL "$Q")"
	echo "$(Field 1 "$Row")"
}

FindPackageName()
{
	Q="SELECT PackageName FROM Software WHERE PK_Software='$1'"
	Row="$(RunSQL "$Q")"
	echo "$(Field 1 "$Row")"
}

NotifyOrbiter()
{
	# notifying Orbiter if asked
	if [[ "$OrbiterID" != "" ]]; then
		/usr/pluto/bin/MessageSend "$DCERouter" 0 $OrbiterID 1 14 15 "*"
	fi
}

SendMessageToOrbiter()
{
	# if Orbiter ID is present
	if [[ "$OrbiterID" != "" ]]; then
		/usr/pluto/bin/MessageSend "$DCERouter" 0 $OrbiterID 1 809 9 "$1" 70 "add_software" 182 "5" 251 0
	fi
}

MyLog "======================================="
MyLog "Started"

if [ ! -d "$ASH_Cache" ]; then
    MyLog "[!!!] Add Software cache folder doesn't exist, creating"
    mkdir "$ASH_Cache"
fi

case $Action in
	install|remove)
		DeviceIP="$(FindDeviceIP "$DeviceID")"
		if [[ -z "$DeviceIP" ]]; then
			MyLog "IP for the device $DeviceID is not set"
			exit 1
		fi
	
		MyLog "Launching $Action on $DeviceIP"
		/usr/pluto/bin/LaunchRemoteCmd.sh "$DeviceIP" "/usr/pluto/bin/AddSoftwareHelper.sh $Action-actual $DeviceID $PackageID $OrbiterID"

		NotifyOrbiter
	;;
	
	
	install-actual)
		_PK_Device="$PK_Device"
		if [[ "$_PK_Device" == "" ]] ;then
			_PK_Device="1"
		fi
		
		# selecting Distro as Model for this device, cutting the LMCE_CORE_ and LMCE_MD_ prefix
		Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device=$_PK_Device AND FK_DeviceData=233"
		Row="$(RunSQL "$Q")"
		Distro=$(Field 1 "$Row")
		Distro=`echo $Distro | sed 's/LMCE_CORE_//' | sed 's/LMCE_MD_//'`

		MyLog "Using Distro $Distro"
		PackageName="$(FindPackageName "$PackageID")"

                # checking internet connection
		/usr/pluto/bin/CheckInternetConnection.sh
                if [[ $? != 0 ]] ;then
                        Q="UPDATE Software_Device SET Status='N' WHERE FK_Device=$DeviceID AND FK_Software=$PackageID"
                        Row="$(RunSQL "$Q")"
                        SendMessageToOrbiter "Installation of package \"$PackageName\" aborted: no internet connection"
			MyLog "No internet connection available, aborting"
                        exit 5
                fi
		
		#getting sources for packages
		Q="SELECT COUNT(*) FROM Software_Source WHERE Distro='$Distro' AND FK_Software=$PackageID AND Virus_Free=1"
		#echo $Q
		Row="$(RunSQL "$Q")"
		SourcesCount="$(Field 1 "$Row")"
		MyLog "Found $SourcesCount sources for package $PackageName"
	
		Err="Download of valid source for package failed"

		for i in `seq 1 $SourcesCount`; do
			SourcesShift=$(( i - 1 ))
			Q="SELECT Downloadurl,Sum_md5,Sum_sha,PK_Software_Source,Version+0 AS Software_Version FROM Software_Source WHERE Distro='$Distro' AND FK_Software=$PackageID AND Virus_Free=1 ORDER BY Software_Version DESC LIMIT $SourcesShift,1"
			#echo $Q
			Row="$(RunSQL "$Q")"
			DownloadURL="$(Field 1 "$Row")"
			MD5Sum="$(Field 2 "$Row")"
			SHA1Sum="$(Field 3 "$Row")"
			PK_Software_Source="$(Field 4 "$Row")"

			CachedFile=`basename $DownloadURL`
			CachedFile="$ASH_Cache/$CachedFile"
			CachedFile=`echo "$CachedFile" | sed 's/\.zip$/\.deb/'`

			MyLog "Checking cache for file $CachedFile"

			if [ -f "$CachedFile" ]; then
			    cp "$CachedFile" "/tmp/$PackageName.deb"
			    MyLog "Found in cache"
			    RetCode=0
			    FromCache="y"
			else
			    MyLog "Not found in cache, attempting to download"
			    MyLog "Trying source $i from $SourcesCount : $DownloadURL"
			    wget -t 5 -T 60 -O "/tmp/$PackageName.deb" "$DownloadURL"
			    RetCode=$?
			    FromCache="n"
			fi

			if [ "$RetCode" -eq 0 ]; then
			    if [[ "$FromCache" == "n" ]]; then
				MyLog "Package downloaded successfully"
			    fi

			    MyLog "Testing checksums.."

				MD5Package=`md5sum /tmp/$PackageName.deb | cut -f1 -d' '`
				SHA1Package=`sha1sum /tmp/$PackageName.deb | cut -f1 -d' '`
				
				if [[ ($MD5Package == $MD5Sum)&&($SHA1Package == $SHA1Sum) ]]; then
					MyLog "Installing package from file /tmp/$PackageName.deb"
					WaitLock "InstallNewDevice" "AddSoftwareHelper"
					
					dpkg -i /tmp/$PackageName.deb
					RetCode=$?
			
					# updating DB only if install was OK
					if [ "$RetCode" -eq 0 ]; then
						Q="INSERT INTO Software_Device(FK_Software,FK_Software_Source, Status, FK_Device) VALUES($PackageID,$PK_Software_Source, 'I', $DeviceID) ON DUPLICATE KEY UPDATE FK_Software_Source=$PK_Software_Source, Status='I'"
						Row="$(RunSQL "$Q")"
						MyLog "Package installed successfully"
					else
						Q="UPDATE Software_Device SET Status='N' WHERE FK_Device=$DeviceID AND FK_Software=$PackageID"
						Row="$(RunSQL "$Q")"
						MyLog "Package installation failed, installer returned non-zero code ($RetCode)"
					fi

					if [[ "$FromCache" == "n" ]]; then
					    MyLog "Adding package to cache"
					    cp -f "/tmp/$PackageName.deb" "$CachedFile"
					fi

					Unlock "InstallNewDevice" "AddSoftwareHelper"
					Err=""
					break
				else
					MyLog "Checksums test failed for this package: MD5=$MD5Package (should be $MD5Sum), SHA1=$SHA1Package (should be $SHA1Sum)"
					if [[ "$FromCache" == "y" ]]; then
					    MyLog "Deleting package from cache"
					    rm -f "$CachedFile"
					fi
					MyLog "Trying next source.."
				fi
			else
				MyLog "Download failed (direct link)"
				Err="failed to download package"
			fi
			
		done

		if [ -n "$Err" ]; then
			Q="UPDATE Software_Device SET Status='N' WHERE FK_Device=$DeviceID AND FK_Software=$PackageID"
			Row="$(RunSQL "$Q")"
			SendMessageToOrbiter "Installation of package \"$PackageName\" aborted: $Err"
		else
			SendMessageToOrbiter "Installation of package \"$PackageName\" finished"
		fi

		[ -n "$Err" ] && exit 10 || exit 0
	;;

	remove-actual)
		# getting package name
		PackageName="$(FindPackageName "$PackageID")"
		if [[ -z "$PackageName" ]]; then
			MyLog "Package with ID=$PackageID is not found"
			exit 10
		fi
		
		MyLog "Removing package $PackageName"
		WaitLock "RemoveNewDevice"
		
		dpkg --remove $PackageName
		
		RetCode=$?
		
		if [ "$RetCode" -eq 0 ]; then
			MyLog "Package removed OK"
		else
			MyLog "Package removing error: installer returned non-zero code ($RetCode), marking package as non-installed anyway"
			Err="yes"
		fi
		
		Q="UPDATE Software_Device SET Status='N' WHERE FK_Device=$DeviceID AND FK_Software=$PackageID"
		Row="$(RunSQL "$Q")"
		
		Unlock "RemoveNewDevice"

		SendMessageToOrbiter "Package \"$PackageName\" removed"	
	
		[ -n "$Err" ] && exit 10 || exit 0
	;;
	
	*)
		echo "Incorrect syntax, please use: $0 (install|install-actual|remove|remove-actual) DeviceID PackageID [OrbiterID]"
		echo "where action is executed for software package PackageID on machine DeviceID, and orbiter OrbiterID is notified at the end"
	;;
esac


