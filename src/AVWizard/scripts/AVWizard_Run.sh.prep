#!/bin/bash

function XorgConfLogging() {
        local message="$1"
        local xorgLog="/var/log/pluto/xorg.conf.log"
        local xorgLines=$(cat /etc/X11/xorg.conf | wc -l)
				
        local myPid=$$

        echo "$myPid $(date -R) $message [$xorgLines]"  >> $xorgLog
}

ConfFile="/etc/X11/xorg.conf"
XorgConfLogging "Starting $0 $*"
trap 'XorgConfLogging "Ending"' EXIT

DEVICETEMPLATE_OnScreen_Orbiter=62
DEVICETEMPLATE_OrbiterPlugin=12

DEVICECATEGORY_Media_Director=8

DEVICEDATA_ScreenWidth=100
DEVICEDATA_ScreenHeight=101
DEVICEDATA_PK_Size=25
DEVICEDATA_Video_settings=89
DEVICEDATA_Connector=68
DEVICEDATA_Spacing=150
DEVICEDATA_Offset=167
DEVICEDATA_TV_Standard=229

COMMANDPARAMETER_PK_Device=2
COMMANDPARAMETER_Force=21
COMMANDPARAMETER_Reset=24

UI_Normal_Horizontal=1
UI_V2_Normal_Horizontal=4

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/X-Common.sh

. /usr/pluto/bin/TeeMyOutput.sh --outfile /var/log/pluto/AVWizard.log --infile /dev/null --stdboth -- "$@"

UseAlternativeLibs

CleanUp()
{
	rm -f /tmp/*.xml
}

SetupX()
{
	rm -f "$XF86Config"
	if [[ -f "$ConfFile" ]]; then
		cp "$ConfFile" "$XF86Config"
	fi
	rmmod nvidia &>/dev/null || :
	# default test
	bash -x "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --resolution '640x480@60' --no-test | tee-pluto /var/log/pluto/Xconfigure.log
	if ! TestXConfig "$Display" "$XF86Config"; then
		# vesa test
		bash -x "$BaseDir"/Xconfigure.sh --conffile "$XF86Config" --resolution '640x480@60' --force-vesa --no-test | tee-pluto /var/log/pluto/Xconfigure.log
		if ! TestXConfig "$Display" "$XF86Config"; then
			# all tests failed
			beep -l 350 -f 300 &
			chvt 8
			whiptail --msgbox "Failed to setup X" 0 0 --title "AVWizard" --clear </dev/tty8 &>/dev/tty8
		fi
	fi
	sed -ir '/Option.*"Composite"/d' "$XF86Config"
	WMTweaksFile="/root/.config/xfce4/mcs_settings/wmtweaks.xml"
	cp "$WMTweaksFile"{,.orig}
	sed -i '/Xfwm\/UseCompositing/ s/value="."/value="1"/g' "$WMTweaksFile"
}

SetDefaults()
{
	WizSet Video_Ratio '4_3'
	WizSet Resolution '640x480'
	WizSet VideoResolution '640x480'
	WizSet WindowWidth 640
	WizSet WindowHeight 480
	WizSet Refresh '60'
	WizSet VideoRefresh '60'
	WizSet VideoOutput 'VGA'
	WizSet ResolutionSelected 1
	WizSet AudioConnector 'Analog Stereo'
	GetAudioMixerSettings
	WizSet AudioVolumeMin "$AudioVolumeMin"
	WizSet AudioVolumeMax "$AudioVolumeMax"
	WizSet AudioVolumeCurrent "$AudioVolumeCurrent"
	WizSet AudioVolumeIncrement '1'
	WizSet DolbyTest '0'
	WizSet DTSTest '0'
	WizSet XineConfigFile "$XineConf"
	cp /etc/pluto/xine.conf "$XineConf"
	WizSet SoundTestFile '/home/public/data/avwizard-sounds/avwizard_volume_test.mp3'
	WizSet DTSTestFile '/home/public/data/avwizard-sounds/avwizard_dts_test.wav'
	WizSet DolbyTestFile '/home/public/data/avwizard-sounds/avwizard_ac3_test.ac3'
	WizSet DefaultFontName '/usr/share/fonts/truetype/msttcorefonts/Arial.ttf'
	WizSet RemoteAVWizardServerPort "$AVWizard_Port"
	WizSet RemoteCmd "$RemoteCmd"
	WizSet ExitCode 1 # by default, we fail (i.e. on Ctrl+Alt+Bkspc)
	echo "640x480 60 640 480 VGA" >/tmp/avwizard-resolution.txt
}

# Update audio settings of this machine
UpdateAudioSettings()
{
	DEVICEDATA_Audio_Settings=88
	DEVICECATEGORY_Media_Director=8

	Q="
		SELECT FK_DeviceCategory
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='$PK_Device'
	"
	Category=$(RunSQL "$Q")

	if [[ -z "$Category" ]]; then
		return 1
	elif [[ "$Category" != "$DEVICECATEGORY_Media_Director" ]]; then
		# it's the Core; search for its embedded MD
		Q="
			SELECT PK_Device
			FROM Device
			JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
			JOIN DeviceCategory ON FK_DeviceCategory=PK_DeviceCategory
			WHERE FK_Device_ControlledVia='$PK_Device' AND PK_DeviceCategory='$DEVICECATEGORY_Media_Director'
		"
		MD=$(RunSQL "$Q")
		if [[ -z "$MD" ]]; then
			# No embedded MD found
			return 1
		fi
	else
		MD="$PK_Device"
	fi

	WizAudioConnector=$(WizGet 'AudioConnector')
	WizAC3_Result=$(WizGet 'DolbyTest')
	WizDTS_Result=$(WizGet 'DTSTest')

	case "$WizAudioConnector" in
		'Analog Stereo')
			AudioOutput="S"
		;;
		'Analog Multi-channel')
			AudioOutput="L"
		;;
		'SPDIF Coaxial')
			AudioOutput="C"
		;;
		'SPDIF Optical')
			AudioOutput="O"
		;;
	esac
	
	# if at least one of the digital streams passed the test, enable passthrough
	if [[ "$WizAC3_Result" != 0 || "$WizDTS_Result" != 0 ]]; then
		PassThrough="3"
	fi

	NewAudioSetting="$AudioOutput$PassThrough"

	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('$MD', '$DEVICEDATA_Audio_Settings', '$NewAudioSetting')
	"
	RunSQL "$Q"
}

UpdateOrbiterDimensions()
{
	WizResolution=$(< /tmp/avwizard-resolution.txt)
	VideoResolution_Name=$(SpcField 1 "$WizResolution")
	VideoRefresh=$(SpcField 2 "$WizResolution")
	WindowWidth=$(SpcField 3 "$WizResolution")
	WindowHeight=$(SpcField 4 "$WizResolution")
	Connector=$(SpcField 5 "$WizResolution")
	TVStandard=$(SpcField 6 "$WizResolution")

	ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
	OrbiterDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_OnScreen_Orbiter")
	OrbiterResolutionName="$VideoResolution_Name"
	OrbiterRefresh="$VideoRefresh"
	OrbiterResolutionFullName=$(Resolution_GetFullName "$OrbiterResolutionName")
	if [[ "$OrbiterResolutionFullName" == *=* ]]; then
		OrbiterResolution=${OrbiterResolutionFullName#*=}
		if [[ "$OrbiterResolution" == *i ]]; then
			Iletter=" i"
		fi
		OrbiterResolution=${OrbiterResolution%i}
		OrbiterResolution=${OrbiterResolution%p}
	else
		OrbiterResolution="$OrbiterResolutionName"
	fi
	
	OrbiterWidth=${OrbiterResolution%x*}
	OrbiterHeight=${OrbiterResolution#*x}

	OrbiterBorder=$(WizGet 'WizardBorder')
	OrbiterLeft=$(WizGet 'LeftBorder')
	OrbiterTop=$(WizGet 'TopBorder')

	ReducePercent=$(echo "2 * $OrbiterBorder / $OrbiterWidth * 100" | bc -l | cut -d. -f1)
	OrbiterShiftX=$((OrbiterLeft-OrbiterBorder))
	OrbiterShiftY=$((OrbiterTop-OrbiterBorder))

	# Store screen width and hight
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterWidth' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenWidth'"
	RunSQL "$Q"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterHeight' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_ScreenHeight'"
	RunSQL "$Q"

	# Store value for "Video settings"
	Video_settings="$OrbiterWidth $OrbiterHeight$Iletter/$OrbiterRefresh"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$Video_settings' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings'"
	RunSQL "$Q"

	# Store value for "Video output connector"
	Video_Connector="$Connector"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$Video_Connector' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Connector'"
	RunSQL "$Q"
	
	# Store value for "TV Standard"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$TVStandard' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_TV_Standard'"
	RunSQL "$Q"

	# Store PK_Size
	Q="SELECT PK_Size FROM Size WHERE Description LIKE '%$OrbiterResolutionName%'"
	PK_Size=$(RunSQL "$Q")
	if [[ -n "$PK_Size" ]]; then
		Q="UPDATE Device_DeviceData SET IK_DeviceData='$PK_Size' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_PK_Size'"
		RunSQL "$Q"
	fi

	# Store value for "Reduce image size by %" (DeviceData 150, "Spacing")
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$ReducePercent' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_Spacing'"
	RunSQL "$Q"

	# Store value for "Offset"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$OrbiterShiftX,$OrbiterShiftY' WHERE FK_Device='$OrbiterDev' AND FK_DeviceData='$DEVICEDATA_Offset'"
	RunSQL "$Q"

	# Orbiter Regen
	Q="
		UPDATE Orbiter
		SET Regen=1
		WHERE PK_Orbiter='$OrbiterDev'
	"
	RunSQL "$Q"
}

UpdateOrbiterUI()
{
	OrbiterDev=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_OnScreen_Orbiter")
	UIVersion=$(WizGet 'UIVersion')
	case "$UIVersion" in
		UI1)
			# disable: OpenGL effects, Alpha blending; select UI1
			UI_SetOptions "$OrbiterDev" 0 0 "$UI_Normal_Horizontal"
		;;
		UI2_med)
			# enable: OpenGL effects; disable: Alpha blending; select UI2
			UI_SetOptions "$OrbiterDev" 1 0 "$UI_V2_Normal_Horizontal"
		;;
		UI2_hi)
			# enable: OpenGL effects, Alpha blending; select UI2
			UI_SetOptions "$OrbiterDev" 1 1 "$UI_V2_Normal_Horizontal"
		;;
		*)
			echo "Unknown UIVersion value: '$UIVersion'"
		;;
	esac
}

SetupViaXine()
{
	local Q OrbiterDev XineDev
#<-mkr_b_via_b->
	Q="
		SELECT PK_Device
		FROM Device
		WHERE FK_DeviceTemplate=62 AND FK_Device_ControlledVia=$PK_Device
	"
	OrbiterDev="$(RunSQL "$Q")"

	Q="
		SELECT PK_Device
		FROM Device
		WHERE FK_DeviceTemplate=5 AND FK_Device_ControlledVia=$OrbiterDev
	"
	XineDev="$(RunSQL "$Q")"

	# Hardware acceleration
	RunSQL "
		UPDATE Device_DeviceData
		SET IK_DeviceData='cle266x11'
		WHERE FK_DeviceData=168 AND FK_Device=$XineDev
	"
#<-mkr_b_via_e->
}

ConfSet AVWizardOverride 0
rm -f /tmp/AVWizard_Started

SetupViaXine

RemoteCmd=$(/usr/pluto/bin/AVWizard_Remote_Detect.sh | tail -1)

Done=0
while [[ "$Done" -eq 0 ]]; do
	echo "$(date -R) $(basename "$0"): AVWizard Main loop"
	CleanUp
	SetupX
	"$BaseDir"/AVWizard-XineDefaultConfig.sh
	SetDefaults
	"$BaseDir"/AVWizardWrapper.sh
	mv "$WMTweaksFile"{.orig,}
	Ret="$?"
	echo "$(date -R) $(basename "$0"): AVWizard Main loop ret code $Ret"
	case "$Ret" in
		0) Done=1 ;;
		3)
			rm -f "$XF86Config" "$XineConf"
			exit 0 ;; # AV Wizard canceled; don't apply any changes
		*) : ;; # All others re-loop (failures)
	esac
done

echo "$(date -R) $(basename "$0"): AVWizard Main loop done"

set -x
# Finalize wizard: save settings
ConfSet "AVWizardDone" "1"
mv "$XF86Config" "$ConfFile"
mv "$XineConf" /etc/pluto/xine.conf
echo "$(date -R) $(basename "$0"): AVWizard reset conf file"
alsactl store
echo "$(date -R) $(basename "$0"): AVWizard Calling Audio Settings"
UpdateAudioSettings
echo "$(date -R) $(basename "$0"): AVWizard Calling UpdateOrbiterDimensions"
UpdateOrbiterDimensions
echo "$(date -R) $(basename "$0"): AVWizard Calling UpdateOrbiterUI"
UpdateOrbiterUI
echo "$(date -R) $(basename "$0"): AVWizard Calling SetupAudioVideo"
bash -x /usr/pluto/bin/SetupAudioVideo.sh | tee-pluto /var/log/pluto/avwizard_setup_av.log
set +x

ConfSet AVWizardOverride 0
