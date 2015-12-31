#!/bin/bash

set -x
exec &>/tmp/NuForce.log

. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh

DEVICECATEGORY_Media_Director=8

DEVICETEMPLATE_Media_Plugin=2
DEVICETEMPLATE_OnScreen_Orbiter=62
DEVICETEMPLATE_Asterisk=45
DEVICETEMPLATE_MythTV_Player=35
DEVICETEMPLATE_Orbiter_Embedded_Phone=1759

DEVICEDATA_ScreenWidth=100
DEVICEDATA_ScreenHeight=101
DEVICEDATA_PK_Skin=24
DEVICEDATA_PK_Size=25
DEVICEDATA_Video_settings=89
DEVICEDATA_Connector=68
DEVICEDATA_TV_Standard=229
DEVICEDATA_Audio_Settings=88
DEVICEDATA_Type=47

ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
OrbiterDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_OnScreen_Orbiter")
MediaPluginDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Media_Plugin")

Packages()
{
	RemovePkgs=(
		asterisk asterisk-config asterisk-mysql asterisk-pluto asterisk-sounds-main freepbx pluto-asterisk
		asterisk-perl libct3 libiksemel3 libpri1.2 libradiusclient-ng2 libsqlite0 libtonezone0 php-db php-pear
		pluto-proxy-orbiter pluto-voicemail-monitor zaptel-modules-2.6.22-14-generic
		mythtv-backend mythtv-common mythtv-database mythtv-frontend mythtv-transcode-utils mythweb
		pluto-mythtv-player pluto-mythtv-plugin libmyth-0.20 video-wizard-videos
		chan-sccp libtonezone1 traceroute
	)

	InstallPkgs=(
		id-my-disc egalax elo-touchscreen 3m-touchware pluto-slim-server-streamer
	)

	for Pkg in "${RemovePkgs[@]}"; do
		apt-get -y -f --purge remove "$Pkg"
	done

	for Pkg in "${InstallPkgs[@]}"; do
		apt-get -y -f install "$Pkg"
	done
}

Devices()
{
	/usr/pluto/bin/CreateDevice -d 53 -R 1 # Slim Server Streamer

CreateDiskDrive='#!/bin/bash
trap "rm -f /etc/rc2.d/S91firstboot-nuforce" EXIT
CDROMs=$(hal-find-by-property --key storage.drive_type --string cdrom)
for UDI in $CDROMs; do
	Device=$(hal-get-property --udi "$UDI" --key block.device)
	/usr/pluto/bin/CreateDevice -d 11 -R 1 -A "6|$Device|161|$UDI" # Disk Drive
done
'
	echo -n "$CreateDiskDrive" >/etc/rc2.d/S91firstboot-nuforce
	chmod +x /etc/rc2.d/S91firstboot-nuforce
}

AVWizardReplacement()
{
	UI_Normal_Horizontal=1

	OrbiterWidth=1024
	OrbiterHeight=768
	Video_settings="$OrbiterWidth $OrbiterHeight/60"
	Video_Connector="VGA"
	PK_Skin=12 # NuForce
	PK_Size=6 # 1024x768
	NewAudioSetting="C3"

	Queries=(
		"UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterWidth' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenWidth'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterHeight' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenHeight'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$Video_settings' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$Video_Connector' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Connector'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$PK_Skin' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_PK_Skin'"
		"UPDATE Device_DeviceData SET IK_DeviceData='$PK_Size' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_PK_Size'"
		"UPDATE Orbiter SET Regen=1 WHERE PK_Orbiter='$OrbiterDev'"
		"REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData) VALUES('$ComputerDev', '$DEVICEDATA_Audio_Settings', '$NewAudioSetting')"
	)

	for Q in "${Queries[@]}"; do
		RunSQL "$Q"
	done

	UI_SetOptions "$OrbiterDev" 0 0 "$UI_Normal_Horizontal"

	/usr/pluto/bin/Xconfigure.sh --force-vesa --defaults

AVWizardRun="#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

cp /etc/X11/xorg.conf{.nuforce,}
amixer sset IEC958 unmute
amixer sset Master 81% unmute
amixer sset PCM 100% unmute
amixer sset Front 100% unmute
alsactl store

#xinit /usr/bin/TouchKit -- :0 -ac #Egalax
xinit /elo/elova -u -- :0 -ac #ELO

/etc/init.d/elorc start
/etc/init.d/TWDrvStartup start

ConfSet AVWizardDone 1
"

Xconfigure="#!/bin/bash
cp /etc/X11/xorg.conf{.nuforce,}
"


	echo -n "$AVWizardRun" >/usr/pluto/bin/AVWizard_Run.sh
	echo -n "$Xconfigure" >/usr/pluto/bin/Xconfigure.sh
	echo "#!/bin/bash" >/usr/pluto/bin/X-CheckSettings.sh

	chmod +x /usr/pluto/bin/{AVWizard_Run,Xconfigure,X-CheckSettings}.sh

	ConfSet AVWizardDone 0
	ConfSet UseVideoWizard 0
}

AddEgalaxToXorgConf()
{
	Section='
Section "InputDevice"
	Identifier "EETI"
	Driver "egalax"
	Option "Device" "events"
	Option "Parameters" "/etc/egalax.cal"
	Option "ScreenNo" "0"
EndSection
'

	echo "$Section" >>/etc/X11/xorg.conf

	ServerLayout=NoServerLayout
	while read line; do
		line="${line#+}"
		line="${line%+}"
		if [[ "$line" == *'Section "ServerLayout"'* ]]; then
			ServerLayout=ServerLayout
		fi
		if [[ "$ServerLayout" == ServerLayout && "$line" == *"EndSection"* ]]; then
			ServerLayout=NoServerLayout
			echo $'\tInputDevice "EETI" "SendCoreEvents"'
		fi
		echo "$line"
	done < <(cat /etc/X11/xorg.conf | sed -r 's,^|$,+,g') >/etc/X11/xorg.conf.nuforce
}

DatabaseDefaults()
{
	Queries=(
		"UPDATE Device_DeviceData SET IK_DeviceData='flac' WHERE FK_Device='$MediaPluginDev' AND FK_DeviceData='$DEVICEDATA_Type'"
		"INSERT INTO Room(FK_Installation, FK_RoomType, Description) VALUES('$PK_Installation', 1, 'NuForce')"
		"UPDATE Device SET FK_Room=1 WHERE PK_Device=1"
		"UPDATE Device_DeviceData SET IK_DeviceData='' WHERE FK_DeviceData=28"
		"INSERT INTO Users(UserName,PINCode) VALUES('nuforce','1234')"
		"INSERT INTO Installation_Users(FK_Installation, FK_Users, userCanModifyInstallation, userCanChangeHouseMode)
			VALUES(1, 1, 1, 1)"
		"DELETE FROM Device_QuickStart WHERE Description like '%MythTV%'"
		"UPDATE Device SET IPAddress='192.168.78.1' WHERE IPAddress='192.168.80.1'"
		"UPDATE Firewall SET SourceIP='192.168.78.1' WHERE SourceIP='192.168.80.1'"
		"UPDATE Firewall SET DestinationIP='192.168.78.1' WHERE DestinationIP='192.168.80.1'"
	)

	for Q in "${Queries[@]}"; do
		RunSQL "$Q"
	done

	Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate IN
		($DEVICETEMPLATE_Asterisk, $DEVICETEMPLATE_MythTV_Player, $DEVICETEMPLATE_Orbiter_Embedded_Phone)"
	R=$(RunSQL "$Q")
	for Device in $R; do
		DeleteDevice "$Device"
	done

	/usr/pluto/bin/Timezone_Detect.sh
	/usr/pluto/bin/SetPasswords.sh 1 nuforce
}

Firewall()
{
	Ports=(
		tcp:22 tcp:80
		udp:137 udp:138 tcp:139 tcp:445
	)
	for Port in "${Ports[@]}"; do
		Protocol="${Port%:*}"
		Port="${Port#*:}"
		Q="
			INSERT INTO Firewall(Protocol, SourcePort, RuleType)
			VALUES('$Protocol', '$Port', 'core_input');
		"
		RunSQL "$Q"
	done
}

ApplyHacks()
{
	AsoundSubst='s/convert48k/convert44k/g; s/rate 48000/rate 44100/g; /playback\.pcm/ s/spdif_playback/plughw:0,0/g'
	AsoundCleanup='/pcm_slave.convert44k {/,/^$/ {next}; /pcm.spdif_playback {/,/^$/ {next} {print}'

	sed -ri "$AsoundSubst" /usr/pluto/templates/asound.conf
	awk "$AsoundCleanup" /usr/pluto/templates/asound.conf >/usr/pluto/templates/asound.conf.nuforce
	mv /usr/pluto/templates/asound.conf{.nuforce,}

	if [[ -f /etc/asound.conf ]]; then
		sed -ri "$AsoundSubst" /etc/asound.conf
		awk "$AsoundCleanup" /etc/asound.conf >/etc/asound.conf.nuforce
		mv /etc/asound.conf{.nuforce,}
	fi
	
	echo "pcm.!default asym_spdif" >> /etc/asound.conf
	
}

Packages
Devices
AVWizardReplacement
AddEgalaxToXorgConf
DatabaseDefaults
Firewall
ApplyHacks
exit 0
