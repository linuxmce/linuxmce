#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

DEVICECATEGORY_Media_Director=8
DEVICECATEGORY_Video_Cards=125
DEVICECATEGORY_Sound_Cards=124

DEVICETEMPLATE_OnScreen_Orbiter=62

DEVICEDATA_Video_settings=89
DEVICEDATA_Audio_settings=88
DEVICEDATA_Reboot=236
DEVICEDATA_Connector=68
DEVICEDATA_TV_Standard=229
DEVICEDATA_Setup_Script=189
DEVICEDATA_Sound_Card=288

SettingsFile=/etc/pluto/lastaudiovideo.conf

# don't let KDE override xorg.conf
rm -f {/home/*,/root}/.kde/share/config/displayconfigrc

Reboot=NoReboot
ReloadX=NoReloadX
AudioSetting_Override="$1"
SoundCard_Override="$2"
XineConf_Override="$3"

ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
OrbiterDev=$(FindDevice_Template "$ComputerDev" "$DEVICETEMPLATE_OnScreen_Orbiter")
VideoCardDev=$(FindDevice_Category "$ComputerDev" "$DEVICECATEGORY_Video_Cards")
if [[ -z "$VideoCardDev" ]]; then
	VideoCardDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Video_Cards")
fi

SoundCardDev=$(FindDevice_Category "$ComputerDev" "$DEVICECATEGORY_Sound_Cards")
if [[ -z "$SoundCardDev" ]]; then
	SoundCardDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Sound_Cards")
fi

ReadConf()
{
	## config file format (direcly parsable by bash):
	# name=value
	if [[ -f "$SettingsFile" ]]; then
		while read line; do
			eval "OldSetting_$line"
		done <"$SettingsFile"
	fi
}

GetVideoSetting()
{
	local Q
	local VideoSetting
	
	VideoSetting=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Video_settings")

	if [[ -n "$VideoSetting" ]]; then
		Refresh=$(echo $VideoSetting | cut -d '/' -f2)
		ResolutionInfo=$(echo $VideoSetting | cut -d '/' -f1)
		ResX=$(echo $ResolutionInfo | cut -d' ' -f1)
		ResY=$(echo $ResolutionInfo | cut -d' ' -f2)
		if [[ -z "$Refresh" || -z "$ResX" || -z "$ResY" ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "SetupAudioVideo.sh" "Malformed DeviceData: VideoSetting='$VideoSetting'"
			MalformedVideoSetting='Malformed VideoSetting'
		fi
	fi

	if [[ -z "$MalformedVideoSetting" ]]; then
		echo "$VideoSetting"
	fi
}

SaveSettings()
{
	local Var VarName

	for Var in ${!OldSetting_*}; do
		VarName="${Var#OldSetting_}"
		eval "Save_$VarName=\"${!Var}\""
	done

	for Var in ${!NewSetting_*}; do
		VarName="${Var#NewSetting_}"
		eval "Save_$VarName=\"${!Var}\""
	done

	for Var in ${!Save_*}; do
		VarName="${Var#Save_}"
		echo "$VarName=\"${!Var}\""
	done >"$SettingsFile"
}

EnableDigitalOutputs()
{
	local SoundCard="$1"
	# Added this to correctly unmute channels for setup wizard, and to 
	# inject necessary unmuting commands for later bootup.
	amixdigital=$(amixer -c "$SoundCard" | grep Simple | cut -d' ' -f4,5,6 | sort | uniq)
	for output in $amixdigital; do
		amixer -c "$SoundCard" sset $output unmute
		amixer -c "$SoundCard" sset output 80%
	done 2>/dev/null 
	alsactl store
}

Setup_AsoundConf()
{
	local AudioSetting="$1"
	local SoundCard

	# Do not mess with asound.conf if Audio Setting is set to Manual. This will only happen after the asound.conf has been generated at least once.
	if [[ "$AudioSetting" == "M" ]]; then
		return
	fi

	local Q R
	local MD_Device="$PK_Device"

	Q="SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=$PK_Device"
	R=$(RunSQL "$Q")

	if [[ "$R" == 7 ]]; then
		Q="SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=28 AND FK_Device_ControlledVia=$PK_Device"
		R=$(RunSQL "$Q")
		MD_Device=$(Field 1 "$R")
	fi

	if [[ -n "$SoundCard_Override" ]]; then
		SoundCard="$SoundCard_Override"
	else
		SoundCard=$(GetDeviceData "$MD_Device" "$DEVICEDATA_Sound_Card")
	fi
	SoundCard=$(TranslateSoundCard "$SoundCard")
	if [[ -z "$SoundCard" ]]; then
		SoundCard=0
	fi

	local AnalogPlaybackCard="plug:dmix:$SoundCard"

	sed -r "s,%MAIN_CARD%,$SoundCard,g; s,%ANALOG_PLAYBACK_CARD%,$AnalogPlaybackCard,g" /usr/pluto/templates/asound.conf >/etc/asound.conf

	case "$AudioSetting" in
		*[CO]*)
			# audio setting is Coaxial or Optical, i.e. S/PDIF
			echo 'pcm.!default asym_spdif' >>/etc/asound.conf
			EnableDigitalOutputs "$SoundCard"
		;;
		*H*)
			# audio setting is HDMI
			echo 'pcm.!default asym_hdmi' >>/etc/asound.conf
			EnableDigitalOutputs "$SoundCard"
		;;
		*)
			# audio setting is Stereo or something unknown
			echo 'pcm.!default asym_analog' >>/etc/asound.conf
		;;
	esac

	local GT220=$(lspci -d 10de:0a20)
	local ION2=$(lspci -d 10de:0a64)

	if [[ "$AudioSetting" == H ]] && [[ -n "$GT220" || -n "$ION2" ]]; then
		sed -ri "s;hdmi:$SoundCard;hw:$SoundCard,7;g" /etc/asound.conf
		aplay -Dplughw:$SoundCard,7 /usr/share/sounds/linphone/rings/orig.wav # this initializes the soundcard (you get total silence without this)
	fi

	Setup_XineConf "$AudioSetting" "$SoundCard" "$AnalogPlaybackCard"
}

Setup_XineConf()
{
	local AudioSetting="$1" SoundCard="$2" AnalogPlaybackCard="$3"
	local XineConf=/etc/pluto/xine.conf

	if [[ -n "$XineConf_Override" ]]; then
		XineConf="$XineConf_Override"
	fi

	case "$AudioSetting" in
		*[CO]*)
			XineConfSet audio.device.alsa_front_device asym_spdif "$XineConf"
			XineConfSet audio.device.alsa_default_device asym_spdif "$XineConf"
			XineConfSet audio.device.alsa_passthrough_device "iec958:CARD=$SoundCard,AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2" "$XineConf"
			;;
		*H*)
			XineConfSet audio.device.alsa_front_device asym_hdmi "$XineConf"
			XineConfSet audio.device.alsa_default_device asym_hdmi "$XineConf"
			XineConfSet audio.device.alsa_passthrough_device "hdmi:CARD=$SoundCard,AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2" "$XineConf"
			;;
		*)
			XineConfSet audio.device.alsa_front_device "$AnalogPlaybackCard" "$XineConf"
			XineConfSet audio.device.alsa_default_device "$AnalogPlaybackCard" "$XineConf"
			;;
	esac

	case "$AudioSetting" in
		*3*)
			XineConfSet audio.output.speaker_arrangement 'Pass Through' "$XineConf"
			;;
		*)
			XineConfSet audio.output.speaker_arrangement 'Stereo 2.0' "$XineConf"
			;;
	esac
}

VideoSettings_Check()
{
	local Update_XorgConf="NoXorgConf"
	local DB_VideoSetting DB_OpenGL DB_AlphaBlending DB_Connector DB_TVStandard DB_Reboot

	DB_VideoSetting=$(GetVideoSetting)
	DB_OpenGL=$(OpenGLeffects)
	DB_AlphaBlending=$(AlphaBlendingEnabled)
	DB_Connector=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Connector")
	DB_TVStandard=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_TV_Standard")
	DB_Reboot=$(GetDeviceData "$VideoCardDev" "$DEVICEDATA_Reboot")

	if [[ -n "$DB_VideoSetting" && "$DB_VideoSetting" != "$OldSetting_VideoSetting" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_VideoSetting="$DB_VideoSetting"
	fi
	if [[ -n "$DB_OpenGL" && "$DB_OpenGL" != "$OldSetting_OpenGL" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_OpenGL="$DB_OpenGL"
	fi
	if [[ -n "$DB_AlphaBlending" && "$DB_AlphaBlending" != "$OldSetting_AlphaBlending" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_AlphaBlending="$DB_AlphaBlending"
	fi
	if [[ -n "$DB_Connector" && "$DB_Connector" != "$OldSetting_Connector" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_Connector="$DB_Connector"
	fi
	if [[ -n "$DB_TVStandard" && "$DB_TVStandard" != "$OldSetting_TVStandard" ]]; then
		Update_XorgConf="XorgConf"
		NewSetting_TVStandard="$DB_TVStandard"
	fi

	if [[ "$Update_XorgConf" == "XorgConf" ]]; then
		/usr/pluto/bin/Xconfigure.sh

		if [[ "$DB_Reboot" == 1 ]]; then
			Reboot="Reboot"
		else
			ReloadX="ReloadX"
		fi

		# Orbiter Regen
		Q="
			UPDATE Orbiter
			SET Regen=1
			WHERE PK_Orbiter='$OrbiterDev'
		"
		RunSQL "$Q"
	fi
}

AudioSettings_Check()
{
	local DB_AudioSetting DB_AudioScript DB_Reboot
	local ScriptPath

	if [[ -z "$AudioSetting_Override" ]]; then
		DB_AudioSetting=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Audio_settings")
	else
		DB_AudioSetting="$AudioSetting_Override"
	fi
	DB_Reboot=$(GetDeviceData "$AudioCardDev" "$DEVICEDATA_Reboot")

	Logging "$TYPE" "$SEVERITY_NORMAL" "SetupAudioVideo" "'"

	DB_AudioScript=$(GetDeviceData "$SoundCardDev" "$DEVICEDATA_Setup_Script")
	ScriptPath="/usr/pluto/bin/$DB_AudioScript"

	Logging "$TYPE" "$SEVERITY_NORMAL" "SetupAudioVideo" "DBSetting: $DB_AudioSetting Reboot: $DB_Reboot Script Path: $ScriptPath"

	if [[ -n "$DB_AudioSetting" && -f "$ScriptPath" ]]; then
		Logging "$TYPE" "$SEVERITY_NORMAL" "SetupAudioVideo" "Running: $ScriptPath $DB_AudioSetting"
		"$ScriptPath" "$DB_AudioSetting"
	fi
	NewSetting_AudioSetting="$DB_AudioSetting"

	if [[ "$NewSetting_AudioSetting" == *S* ]]; then
		# S3 is not a valid combination and will break things
		NewSetting_AudioSetting="${NewSetting_AudioSetting//3}"
	fi
	Setup_AsoundConf "$NewSetting_AudioSetting"

	if [[ "$DB_Reboot" == 1 ]]; then
		Reboot="Reboot"
	fi
}

Logging "$TYPE" "$SEVERITY_NORMAL" "SetupAudioVideo" "Starting"

ReadConf
VideoSettings_Check
AudioSettings_Check
SaveSettings

if [[ -z "$(pidof X)" ]]; then
	exit # no X is running
fi

if [[ "$Reboot" == Reboot ]]; then
	reboot
elif [[ "$ReloadX" == ReloadX ]]; then
	/usr/pluto/bin/RestartLocalX.sh &
	disown -a
fi
