#!/bin/bash -x

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/X-Common.sh

###########################################################
### Setup global variables
###########################################################
DEVICETEMPLATE_OnScreen_Orbiter="62"
DEVICETEMPLATE_OrbiterPlugin="12"
DEVICECATEGORY_Media_Director="8"
DEVICEDATA_ScreenWidth="100"
DEVICEDATA_ScreenHeight="101"
DEVICEDATA_PK_Size="25"
DEVICEDATA_Video_settings="89"
DEVICEDATA_Connector="68"
DEVICEDATA_Spacing="150"
DEVICEDATA_Offset="167"
DEVICEDATA_TV_Standard="229"
DEVICEDATA_Sound_card="288"
COMMANDPARAMETER_PK_Device="2"
COMMANDPARAMETER_Force="21"
COMMANDPARAMETER_Reset="24"
UI_Normal_Horizontal="1"
UI_V2_Normal_Horizontal="4"
ConfFile="/etc/X11/xorg.conf"
#From /usr/pluto/bin/Utils.sh - Sets LD_Library_Path
UseAlternativeLibs
#Setup Log file variable
log_file="/var/log/pluto/AVWizard_Run_$(date +%Y%m%d_%H%M%S).log"

###########################################################
### Setup Functions - Error checking and logging
###########################################################

VerifyExitCode () {
	local EXITCODE=$?
	if [ "$EXITCODE" != "0" ] ; then
		echo "An error (Exit code ${EXITCODE}) occured during the last action"
		echo "$1"
		exit 1
	fi
}

StatsMessage () {
	printf "`date` - $* \n"
}


###########################################################
### Setup Functions - Reference functions
###########################################################

SetupX () {
	rm -f "$XF86Config"
	if [[ -f "$ConfFile" ]]; then
		cp "$ConfFile" "$XF86Config"
	fi
	rmmod nvidia &>/dev/null || :
	# default test assumes HDMI connection
	bash -x ${BaseDir}/Xconfigure.sh --conffile "$XF86Config" --resolution '1280x720@60' --output HDMI-0 --tv-standard '720p (16:9)' --no-test
	if ! TestXConfig "$Display" "$XF86Config"; then
		# Try a VGA connection with 640x480
		bash -x ${BaseDir}/Xconfigure.sh --conffile "$XF86Config" --resolution '640x480@60' --output VGA --no-test
		if ! TestXConfig "$Display" "$XF86Config"; then
			# vesa test
			bash -x ${BaseDir}/Xconfigure.sh --conffile "$XF86Config" --resolution '640x480@60' --output VGA --force-vesa --no-test
			if ! TestXConfig "$Display" "$XF86Config"; then
				# all tests failed
				beep -l 350 -f 300 &
				chvt 8
				whiptail --msgbox "Failed to setup X" 0 0 --title "AVWizard" --clear </dev/tty8 &>/dev/tty8
			else
				${BaseDir}/AVWizard-XineDefaultConfig.sh
				SetDefaults
				DualBus
			fi
		else
			${BaseDir}/AVWizard-XineDefaultConfig.sh
			SetDefaults
			DualBus
		fi
	else
		${BaseDir}/AVWizard-XineDefaultConfig.sh
		SetDefaults_720p
		DualBus
	fi

	sed -ri '/Option.*"Composite"/d' "$XF86Config"
	WMTweaksFile="/root/.config/xfce4/xfconf/xfce-perchannel-xml/xfwm4.xml"
	cp "$WMTweaksFile"{,.orig}
	sed -i '/Xfwm\/UseCompositing/ s/value="."/value="1"/g' "$WMTweaksFile"
}

DualBus () {
	vga_pci=$(lspci -nn | grep -w 'VGA')
	if [[ $(echo "$vga_pci" | sort -u | wc -l) -gt "1" ]]; then
		CheckVideoDriver # BestGPU
		if [[ -e "$XF86Config" ]]; then
			XorgConfig="$XF86Config"
		else
			XorgConfig="$ConfFile"
		fi

		bus_id=$(echo "$vga_pci" | while IFS=':. ' read -r tok1 tok2 tok3 rest; do printf '%2s %2s %s\n' "$((16#${tok1}))":"$((16#${tok2}))":"$((16#${tok3}))" | sed -e 's/ //g'; done)
		if grep "\#BusID" $XorgConfig; then
			sed -ie "s/\#BusID.*/BusID\t\t\"PCI:${bus_id}\"/g" $XorgConfig
		elif grep "BusID" $XorgConfig; then
			sed -ie "s/BusID.*/BusID\t\t\"PCI:${bus_id}\"/g" $XorgConfig
		elif ! grep "BusID" $XorgConfig; then
			sed -ie "/Driver.*${Best_Video_Driver}/a BusID\t\t\"PCI:${bus_id}\"" $XorgConfig
		fi
	fi
}

SetDefaults () {
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
	WizSet GamepadCmd "$GamepadCmd"
	WizSet ExitCode 1 # by default, we fail (i.e. on Ctrl+Alt+Bkspc)
	echo "640x480 60 640 480 VGA" >/tmp/avwizard-resolution.txt
}

SetDefaults_720p () {
	WizSet Video_Ratio '16_9'
	WizSet Resolution '1280x720'
	WizSet VideoResolution '1280x720'
	WizSet WindowWidth 1280
	WizSet WindowHeight 720
	WizSet Refresh '60'
	WizSet VideoRefresh '60'
	WizSet VideoOutput 'HDMI-0'
	WizSet ResolutionSelected 7
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
	WizSet GamepadCmd "$GamepadCmd"
	WizSet ExitCode 1 # by default, we fail (i.e. on Ctrl+Alt+Bkspc)
	echo "720p 60 1280 720 HDMI-0" >/tmp/avwizard-resolution-defaults.txt
	echo "720p 60 1280 720 HDMI-0" >/tmp/avwizard-resolution.txt
}

UpdateAudioSettings () {
	# Update audio settings of this machine
	DEVICEDATA_Audio_Settings="88"
	DEVICECATEGORY_Media_Director="8"

	Q="
		SELECT FK_DeviceCategory
		FROM Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		WHERE PK_Device='${PK_Device}'
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
			WHERE FK_Device_ControlledVia='${PK_Device}' AND PK_DeviceCategory='${DEVICECATEGORY_Media_Director}'
		"
		MD=$(RunSQL "$Q")
		if [[ -z "$MD" ]]; then
			# No embedded MD found
			return 1
		fi
	else
		MD="$PK_Device"
	fi

	WizSoundCard=$(WizGet 'SoundCard')
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
		'HDMI')
			AudioOutput="H"
		;;
	esac

	# if at least one of the digital streams passed the test, enable passthrough
	if [[ "$WizAC3_Result" != "0" || "$WizDTS_Result" != "0" ]]; then
		PassThrough="3"
	fi

	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('${MD}', '${DEVICEDATA_Sound_card}', '${WizSoundCard}')
	"
	RunSQL "$Q"

	NewAudioSetting="${AudioOutput}${PassThrough}"

	Q="
		REPLACE INTO Device_DeviceData(FK_Device, FK_DeviceData, IK_DeviceData)
		VALUES('${MD}', '${DEVICEDATA_Audio_Settings}', '${NewAudioSetting}')
	"
	RunSQL "$Q"
}

UpdateOrbiterUI () {
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
			echo "Unknown UIVersion value: '${UIVersion}'"
		;;
	esac
}

UpdateOrbiterDimensions () {
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
	Q="UPDATE Device_DeviceData SET IK_DeviceData='${OrbiterWidth}' WHERE FK_Device='${OrbiterDev}' AND FK_DeviceData='${DEVICEDATA_ScreenWidth}'"
	RunSQL "$Q"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='${OrbiterHeight}' WHERE FK_Device='${OrbiterDev}' AND FK_DeviceData='${DEVICEDATA_ScreenHeight}'"
	RunSQL "$Q"

	# Store value for "Video settings"
	Video_settings="$OrbiterWidth ${OrbiterHeight}${Iletter}/${OrbiterRefresh}"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$Video_settings' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings'"
	RunSQL "$Q"

	# Store value for "Video output connector"
	Video_Connector="$Connector"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='$Video_Connector' WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Connector'"
	RunSQL "$Q"

	# Store value for "TV Standard"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='${TVStandard}' WHERE FK_Device='${ComputerDev}' AND FK_DeviceData='${DEVICEDATA_TV_Standard}'"
	RunSQL "$Q"

	# Store PK_Size
	Q="SELECT PK_Size FROM Size WHERE Description LIKE '%${OrbiterResolutionName}%'"
	PK_Size=$(RunSQL "$Q")
	if [[ -n "$PK_Size" ]]; then
		Q="UPDATE Device_DeviceData SET IK_DeviceData='${PK_Size}' WHERE FK_Device='${OrbiterDev}' AND FK_DeviceData='${DEVICEDATA_PK_Size}'"
		RunSQL "$Q"
	fi

	# Store value for "Reduce image size by %" (DeviceData 150, "Spacing")
	Q="UPDATE Device_DeviceData SET IK_DeviceData='${ReducePercent}' WHERE FK_Device='${OrbiterDev}' AND FK_DeviceData='${DEVICEDATA_Spacing}'"
	RunSQL "$Q"

	# Store value for "Offset"
	Q="UPDATE Device_DeviceData SET IK_DeviceData='${OrbiterShiftX},${OrbiterShiftY}' WHERE FK_Device='${OrbiterDev}' AND FK_DeviceData='${DEVICEDATA_Offset}'"
	RunSQL "$Q"

	# Orbiter Regen
	Q="
		UPDATE Orbiter
		SET Regen=1
		WHERE PK_Orbiter='${OrbiterDev}'
	"
	RunSQL "$Q"
}


###########################################################
### Setup Functions - General functions
###########################################################

Video_Driver_Detection () {
	# Check if any driver is installed. If not, check what hardware is in the box, and install the relevant driver using
	# the binary drivers
	CheckVideoDriver
	DriverInstalled="0"
	driverConfig="none"
	grep "Driver" /etc/X11/xorg.conf |grep -v "keyboard" | grep -v "#" | grep -v "vesa" | grep -v "mouse" | grep -vq "kbd" && DriverInstalled="1"
	# Check if we have nVidia or ATI cards in here, and specify proper config programs and commandline options.
	if [[ "$DriverInstalled" -eq "0" ]]; then
		case "$Best_Video_Driver" in
			nvidia)
				driverConfig="nvidia-xconfig"
				driverLine=
			;;
			fglrx)
				driverConfig="aticonfig"
				driverLine="--initial"
			;;
			vboxvideo)
				: # Xorg -configure crashes under vboxvideo for some reason 2014/12/01 - phenigma
			;;
			*)
				driverConfig="Xorg"
				driverLine="-configure"
			;;
		esac
	fi

	if [[ "$driverConfig" != "none" ]]; then
		# Is it installed, if so execute it with any line params
		driverConfigLocation=$(which "$driverConfig")
		if [[ -x "$driverConfigLocation" ]]; then
			$driverConfigLocation $driverLine
			VerifyExitCode "Running $driverConfigLocation $driverLine failed!"
		fi
	fi
}

SetupViaXine() {
	local Q OrbiterDev XineDev
}

GamePad_Setup () {

	if [[ ! -x /usr/pluto/bin/AVWizard_Remote_Detect.sh ]]; then
		chmod +x /usr/pluto/bin/AVWizard_Remote_Detect.sh 
	fi

	if [[ ! -x /usr/pluto/bin/AVWizard_Gamepad_Detect.sh ]]; then
		chmod +x /usr/pluto/bin/AVWizard_Gamepad_Detect.sh
	fi

	RemoteCmd=$(/usr/pluto/bin/AVWizard_Remote_Detect.sh | tail -1)
	GamepadCmd=$(/usr/pluto/bin/AVWizard_Gamepad_Detect.sh | tail -1)

}

OrderAudioModuleLoad () {
# Get module/audio card load order, and convert to hyphen format.
CardOrder=$(cat /proc/asound/modules | awk '{print $2}' | sed 's/_/-/g' | sort -u)
# If no modules loaded, leave. We have no working SoundCards.
if [[ -z "$CardOrder" ]]; then
	return
fi
# Target file alsa-base.conf 
Abase="/etc/modprobe.d/alsa-base.conf"
# Get our current configuration.
CurrentAconfig=$(grep "LinuxMCE" -A20 "$Abase" | grep -v "LinuxMCE" | awk '{print $2}')
if [[ -n "$CurrentAconfig" ]]; then
	# If our module load order does not match our loaded modules, start section fresh.
	if ! [[ "$CardOrder" == "$CurrentAconfig" ]]; then
		sed -i '{/.*Added.*LinuxMCE.*/{/.*End.*LinuxMCE.*/d}};s/.*LinuxMCE.*//g' "$Abase"
		echo "### Added module load order by LinuxMCE ###" >> "$Abase"
	else
		# If they match, leave... our work is done.
		return
	fi
else
	# If there is no current LMCE edit section, create one.
	echo "" >> "$Abase"
	echo "### Added module load order by LinuxMCE ###" >> "$Abase"
fi

# If there is already a positive load order of modules, use that as a base to add a new card in order.
# this allows adding cards. Removing cards could result in an unnecessary module load with unkown results.
ModOrder=$(grep -E "(.*index=[0-9]|.*index=[0-9][0-9])" "$Abase" | sed 's/.*index=//g' | sort -n | tail -1)
if [[ -z "$ModOrder" ]]; then
	ModOrder=0
else
	# Incriment existing highest module order number
	((ModOrder++))
fi

### Loop through loaded audio modules 
echo "$CardOrder" | while read EachModule; do
	Acurrent="options ${EachModule} index"
	# If the current module is given a default do not first value
	# or if the current order number is already assigned (should not happen)
	# then delete the line.
	sed -i "/${Acurrent}=-2*/d;/.*index=${ModOrder}/d" "$Abase"
	# If module does not already have an order number (to prevent multiple loads of the same module
	# or overwriting an existing) write to config file.
	if ! grep -q "${Acurrent}" "$Abase"; then
		echo "${Acurrent}=${ModOrder}" >> "$Abase"
		((ModOrder++))
	fi
done

echo "### End addition by LinuxMCE ###" >> "$Abase"
NewAbase=$(cat -s "$Abase")
echo "$NewAbase" > "$Abase"
echo "" >> "$Abase"

}

Rename_Audio_Cards () {
	CardNumbers=$(cat /proc/asound/modules | awk '{print $1}')
	AudioRules="/etc/udev/rules.d/85-linuxmce-audio.rules"
	cat <<-EOL> "$AudioRules"
		SUBSYSTEM!="sound", GOTO="linuxmce_audio_end"
		ACTION!="add", GOTO="linuxmce_audio_end"
		EOL
	echo "$CardNumbers" | while read each; do
		DevPath=$(udevadm info -a -p /sys/class/sound/card${each} | grep -wo "/devices.*card${each}")
		CardType=$(cat /sys/class/sound/card${each}/id)
		echo "DEVPATH==\"${DevPath}\", ATTR{id}=\"Card${each}_${CardType}\"" >> "$AudioRules"
	done
	cat <<-EOL>> "$AudioRules"
		LABEL="linuxmce_audio_end"
		EOL
}


Enable_Audio_Channels () {
OrderAudioModuleLoad
# Added this to correctly unmute channels for setup wizard, and to 
# inject necessary unmuting commands for later bootup.
yalpa=$(aplay -l)
grep -iwo "card ." <<< "$yalpa" | awk '{print $2}' | uniq | while read CardNumber; do
	amixer -c "$CardNumber" | grep '\[off\]' -B5 | grep "Simple" | sed 's/Simple mixer control //g' | grep -viE '(capture|mic)' | while read MuteStatus; do 
		amixer -c "$CardNumber" sset "$MuteStatus" unmute 
	done
	amixer -c "$CardNumber" | grep '\[.*\%\]' -B5 | grep "Simple" | sed 's/Simple mixer control //g' | grep -viE '(capture|mic)' | while read VolLevel; do 
		amixer -c "$CardNumber" sset "$VolLevel" 80%
	done
done
alsactl store

if grep -qi "device 7" <<< "$yalpa"; then 
	CardNumber=$(grep -i "device 7" <<< "$yalpa" | grep -iwo "card ." | awk '{print $2}')
	aplay -Dplughw:${CardNumber},7 /usr/share/sounds/linphone/rings/orig.wav
fi
}

Start_AVWizard () {
	#Modify /etc/pluto.conf for the AVWizardOverride value
	rm -f /tmp/AVWizard_Started

	#Setup Main AVWizard loop
	Done=0
	while [[ "$Done" -eq "0" ]]; do
		StatsMessage "AVWizard Main loop"
		rm -f /tmp/*.xml
		SetupX
		${BaseDir}/AVWizardWrapper.sh
		Ret="$?"
		mv "$WMTweaksFile"{.orig,}
		StatsMessage "AVWizard Main loop ret code $Ret"
		case "$Ret" in
			0) Done=1 ;;
			3)
				rm -f "$XF86Config" "$XineConf"
				exit 0 ;; # AV Wizard canceled; don't apply any changes
			*) : ;; # All others re-loop (failures)
		esac
	done

	StatsMessage "AVWizard Main loop done"

	# Finalize wizard: save settings
	mv "$XF86Config" "$ConfFile"
	mv "$XineConf" /etc/pluto/xine.conf
	StatsMessage "AVWizard reset conf file"
	alsactl store
	StatsMessage "AVWizard Saving Audio Settings"
	UpdateAudioSettings
	StatsMessage "AVWizard Calling UpdateOrbiterDimensions"
	UpdateOrbiterDimensions
	StatsMessage "AVWizard Calling UpdateOrbiterUI"
	UpdateOrbiterUI
	StatsMessage "AVWizard Calling SetupAudioVideo"
	bash -x /usr/pluto/bin/SetupAudioVideo.sh | tee-pluto /var/log/pluto/avwizard_setup_av.log

	ConfSet "AVWizardDone" "1"
	ConfSet "AVWizardOverride" "0"
}


###########################################################
### Main execution area
###########################################################

#Create AVWizard.log
. /usr/pluto/bin/TeeMyOutput.sh --outfile "$log_file" --infile /dev/null --stdboth -- "$@"

#Trap
trap 'StatsMessage "Exiting"' EXIT

#Execute Functions
Video_Driver_Detection
SetupViaXine
Enable_Audio_Channels
GamePad_Setup
Start_AVWizard

StatsMessage "AVWizard completed without any detected error"
exit 0
