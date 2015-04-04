#!/bin/bash

set -e 

. /usr/pluto/bin/SQL_Ops.sh 2>/dev/null
. /usr/pluto/bin/Section_Ops.sh 2>/dev/null
. /usr/pluto/bin/Config_Ops.sh 2>/dev/null

DEVICEDATA_DisklessBoot=9
DEVICEDATA_PK_Distro=7

DestDir=$(mktemp -d)

function update_config_files
{
	local ScriptDir="/usr/pluto/bin/files.d"
	local ScriptsList="cron.d-synctime interfaces mythtv-mysql.txt nis-client pluto.conf resolv.conf syslog.conf timezone mountnfs.sh apt.conf mythtv-fix hostname rc-shutdown"
	for Script in $ScriptsList ;do
		if [[ ! -x $ScriptDir/$Script ]] ;then
			echo "WARNING: Script $Script cannot be executed"
			continue
		fi

		pushd $ScriptDir
			$ScriptDir/$Script --root $DestDir --device $Moon_DeviceID
		popd
	done
}

function build_installer_script 
{
	mkdir -p "${DestDir}/usr/pluto/install"
	local InstallerFiles="ConfirmDependencies_Debian.sh Common.sh AptSources.sh"
	for file in $InstallerFiles ;do
		cp "/usr/pluto/install/${file}" "${DestDir}/usr/pluto/install"
	done

	Moon_DistroID=$(RunSQL "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$Moon_DeviceID' AND FK_DeviceData='$DEVICEDATA_PK_Distro'")

	/usr/pluto/bin/ConfirmDependencies -o "$Moon_DistroID" -r -D "$MySqlDBName" $PLUTO_DB_CRED -d "$Moon_DeviceID" install > "${DestDir}/usr/pluto/install/activation.sh"
}

function put_sample_movie
{
	mkdir -p "${DestDir}/usr/pluto"
	cp /usr/pluto/sample.mpg "${DestDir}/usr/pluto"
}

function create_archive
{
	pushd ${DestDir}
		tar zcf /usr/pluto/var/Disked_${Moon_DeviceID}.tar.gz *
	popd
}

function setup_mysql_access 
{
	echo "* Setting up MySQL access for MD #${Moon_DeviceID}"
	RunSQL "GRANT ALL PRIVILEGES ON *.* TO '$MySqlUser'@$Moon_IP; GRANT ALL PRIVILEGES ON *.* TO 'eib'@$Moon_IP"
	RunSQL "FLUSH PRIVILEGES"
}

function setup_hosts_file 
{
	echo "* Setting up /etc/hosts"
	local Content=""
	local Q="
		SELECT 
			PK_Device, 
			IPaddress
		FROM 
			Device
			JOIN Device_DeviceData ON PK_Device = FK_Device AND FK_DeviceData = $DEVICEDATA_DisklessBoot
			JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
		WHERE 
			FK_DeviceCategory = '8'
			AND
			FK_Device_ControlledVia IS NULL
			AND
			IK_DeviceData = '0'
	"
	local R=$(RunSQL "$Q")
	local Row
	for Row in $R ;do
		local DeviceID=$(Field 1 "$Row")
		local IP=$(Field 2 "$Row")

		if [[ "$IP" == "" ]] ;then
			continue
		fi

		Content="${Content}${IP}		moon${DeviceID}\n"
	done
	
	PopulateSection "/etc/hosts" "DiskledMD" "$Content"

	## Export hosts file to other computer
	echo | /usr/lib/yp/ypinit -m

	echo "---$Content---"
}


Q="
	SELECT 
		PK_Device, 
		IPaddress
	FROM 
		Device
		JOIN Device_DeviceData ON PK_Device = FK_Device AND FK_DeviceData = $DEVICEDATA_DisklessBoot
		JOIN DeviceTemplate ON FK_DeviceTemplate = PK_DeviceTemplate
	WHERE 
		FK_DeviceCategory = '8'
		AND
		FK_Device_ControlledVia IS NULL
		AND
		IK_DeviceData = '0'
"

R=$(RunSQL "$Q")
for Row in $R ;do
	DeviceID=$(Field "1" "$Row")
	IP=$(Field "2" "$Row");
	[[ -z "$IP" ]] && IP=$(/usr/pluto/bin/PlutoDHCP.sh -d "$DeviceID" -a)
	if [[ -z "$IP" ]]; then
		echo "WARNING : No free IP left to assign for moon$DeviceID"
		continue
	fi

	# Each Device must be granted access to the mysql database
	setup_mysql_access
done

setup_hosts_file
/usr/pluto/bin/Update_StartupScrips.sh
/usr/pluto/bin/Diskless_ExportsNFS.sh
/usr/pluto/bin/sync_pluto2amp.pl
