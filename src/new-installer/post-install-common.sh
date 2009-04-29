#!/bin/bash


function createStartdevicesScript
{

	cat >/usr/pluto/bin/startDevices.sh <<"EOF"
#!/bin/sh


BASEDIR=/usr/pluto/bin
DeviceTemplate_MeDi=28

source $BASEDIR/SQL_Ops.sh


DEVICES=$*


for DEVICE in $DEVICES
do


	QUERY="SELECT ImplementsDCE, IsPlugin, Disabled, IF(PK_DeviceTemplate=${DeviceTemplate_MeDi},1,0) AS IsMeDi, REPLACE(DeviceTemplate.Description,' ','_') AS Description, CommandLine FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE PK_Device=${DEVICE}"

	RESULT=$(RunSQL "$QUERY")
	for ROW in $RESULT
	do
		IMPLEMENTSDCE=$(Field 1 "$ROW")
		ISPLUGIN=$(Field 2 "$ROW")
		DISABLED=$(Field 3 "$ROW")
		ISMEDI=$(Field 4 "$ROW")
		DESCRIPTION=$(Field 5 "$ROW")
		COMMANDLINE=$(Field 6 "$ROW")

#		echo $RESULT, $IMPLEMENTSDCE $ISPLUGIN $DISABLED $ISMEDI $DESCRIPTION \"$COMMANDLINE\"

		if [ "$ISMEDI" != "0" ]
		then
			echo "Device $DEVICE ($DESCRIPTION) is a Media Director"
			continue
		fi



		if [ "$IMPLEMENTSDCE" = "0" ]
		then

			echo "Device $DEVICE ($DESCRIPTION) no DCE, going after the children"

			CHILDREN=""
			QUERY="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia=$DEVICE"
			RESULT=$(RunSQL "$QUERY")
			for ROW in $RESULT
			do
				CHILD=$(Field 1 "$ROW")
				CHILDREN="$CHILDREN $CHILD"
			done


			if [ "$CHILDREN" ]
			then

				$0 $CHILDREN
			else
				echo "Device $DEVICE ($DESCRIPTION) has no children"
			fi

		else

			if [ "$ISPLUGIN" = "0" ]
			then

				if [ "$DISABLED" = "0" ]
				then

					if [ -z "$COMMANDLINE" ]
					then
						COMMANDLINE=$DESCRIPTION
					fi

					if [ -x "$BASEDIR/${COMMANDLINE}" ]
					then

#						echo "/usr/bin/screen -d -m -S ${DESCRIPTION}-${DEVICE} $BASEDIR/Spawn_Device.sh ${DEVICE} ${DCERouter} ${COMMANDLINE}"
						/usr/bin/screen -d -m -S ${DESCRIPTION}-${DEVICE} $BASEDIR/Spawn_Device.sh ${DEVICE} ${DCERouter} ${COMMANDLINE}
						echo "Device $DEVICE ($DESCRIPTION) started  ${COMMANDLINE}"

					else
						echo "Device $DEVICE ($DESCRIPTION) unable to start ${COMMANDLINE}"
					fi



				else
					echo "Device $DEVICE ($DESCRIPTION) is disabled"
				fi


			else
				echo "Device $DEVICE ($DESCRIPTION) is a plugin"
			fi

		fi

	done

done

EOF
	chmod +x /usr/pluto/bin/startDevices.sh

}



function createStartcoreScript
{

	cat >/usr/pluto/bin/startCore.sh <<"EOF"
#!/bin/sh

BASEDIR=/usr/pluto/bin
DeviceTemplate_Core=7

source $BASEDIR/SQL_Ops.sh

$BASEDIR/Config_Device_Changes.sh
$BASEDIR/Start_OrbiterGen.sh
$BASEDIR/UpdateMediaDaemonControl.sh -enable

$BASEDIR/Start_DCERouter.sh

$BASEDIR/checkforRaids.sh
$BASEDIR/UpdateAvailableSerialPorts.sh

QUERY="SELECT PK_Device FROM Device JOIN DeviceTemplate ON Device.FK_DeviceTemplate=DeviceTemplate.PK_DeviceTemplate WHERE PK_DeviceTemplate=${DeviceTemplate_Core}"
RESULT=$(RunSQL "$QUERY")
for ROW in $RESULT
do
	CoreDev=$(Field 1 "$ROW")
	$BASEDIR/startDevices.sh $CoreDev
done

EOF
	chmod +x /usr/pluto/bin/startCore.sh

}



function createCoreInitScript
{

	cat >/etc/init.d/core <<"EOF"
#!/bin/sh

case "$1" in
	start)
		/usr/pluto/bin/startCore.sh
		;;
	stop)

		/usr/pluto/bin/StopCoreServices.sh
		;;

	*)
		echo "Usage: /etc/init.d/core {start|stop}"
		exit 1
		;;
esac

exit 0
EOF
	chmod +x /etc/init.d/core

}



function createLaunchmanagerInitScript
{

	cat >/etc/init.d/launch-manager <<"EOF"
#!/bin/sh
if [ "$1" = "start" ]
then
	/usr/pluto/bin/Startup_Core-Hybrid.sh
fi

EOF
	chmod +x /etc/init.d/launch-manager

}



function restoreKdmInitScript
{

	yes 'n' | mv -iv /etc/init.d/kdm /etc/init.d/launch-manager
	yes 'n' | cp -iv /etc/init.d/kdm.saved /etc/init.d/kdm

}



function setupRunlevel3
{

	rm -rfv /etc/rc3.d/*
	cp -av /etc/rc2.d/* /etc/rc3.d/
	ln -sfv ../init.d/core /etc/rc3.d/S99core
	rm -fv /etc/rc3.d/S99kdm /etc/rc3.d/S990start_avwizard

}



function setupRunlevel4
{

	rm -rfv /etc/rc4.d/*
	cp -av /etc/rc2.d/* /etc/rc4.d/
	ln -sfv ../init.d/core /etc/rc4.d/S99core

}



function setupRunlevel5
{

	rm -rfv /etc/rc5.d/*
	cp -av /etc/rc2.d/* /etc/rc5.d/
	ln -sfv ../init.d/core /etc/rc5.d/S99core
	ln -sfv ../init.d/launch-manager /etc/rc5.d/S99launch-manager
	rm -fv /etc/rc5.d/S99kdm

}



function modifyUpstartFile
{
	local upstart_file="$1"


	sed -e 's/stop\( on runlevel [2-5]\)/start\1/' -i $upstart_file

}


function createIninttabConfig
{

	cat >/etc/inittab <<"EOF"
# WARNING: Do NOT set the default runlevel to 0 (shutdown) or 6 (reboot).
#id:2:initdefault: # KDE
#id:3:initdefault: # Core
#id:4:initdefault: # Core + KDE
id:5:initdefault: # Launch Manager
EOF

}

function addAdditionalTTYStart
{
	echo "start on runlevel 5">>/etc/event.d/tty2
	echo "start on runlevel 5">>/etc/event.d/tty3
	echo "start on runlevel 5">>/etc/event.d/tty4
	
}



function createDefaultEventFile
{

	cat >/etc/event.d/rc-default <<"EOF"
# rc - runlevel compatibility
#
# This task guesses what the "default runlevel" should be and starts the
# appropriate script.

start on stopped rcS

script
	runlevel --reboot || true


	if grep -q -w -- "-s\|single\|S" /proc/cmdline
	then
		RL="S"
	else
		RL="$(sed -n -e 's/.*runlevel=\([1-5]\).*/\1/p' /proc/cmdline || true)"
	fi

	if [ -z "$RL" -a -r /etc/inittab ]
	then
		RL="$(sed -n -e 's/^id:\([1-5]\):initdefault:.*/\1/p' /etc/inittab || true)"
	fi

	if [ -z "$RL" ]
	then
		RL="2"
	fi

	telinit $RL

end script
EOF

}



function CleanCoreSetup
{

	createStartdevicesScript
	createStartcoreScript
	createCoreInitScript

	restoreKdmInitScript

	setupRunlevel3
	setupRunlevel4
	setupRunlevel5

	modifyUpstartFile /etc/event.d/media-center-startup
#	modifyUpstartFile /etc/event.d/pluto-dhcpd-plugin
	if [ -e /etc/event.d/pluto-dhcpd-plugin ]
	then
		mv -fv /etc/event.d/pluto-dhcpd-plugin /var/tmp
#		rm -fv /etc/event.d/pluto-dhcpd-plugin
	fi

	createIninttabConfig
	createDefaultEventFile

}



function asteriskNoloadModule
{
	local module="$1"


	if ! grep -q "^[^;]*noload => ${module}" /etc/asterisk/modules.conf
	then
		sed -e "s/\[global\]/noload => ${module}\n&/" -i /etc/asterisk/modules.conf
	else
		echo "module $module already disabled"
	fi

}



function FixAsteriskConfig
{

	asteriskNoloadModule app_directory_odbc.so
	asteriskNoloadModule app_voicemail_odbc.so
	asteriskNoloadModule app_voicemail_imap.so

}


