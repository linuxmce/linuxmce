#!/bin/bash
. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh

DEVICECATEGORY_Media_Director="8"
DEVICECATEGORY_Video_Cards="125"
DEVICECATEGORY_Sound_Cards="124"
DEVICETEMPLATE_OnScreen_Orbiter="62"
DEVICETEMPLATE_Stereo_virtual_sound_card="2242"
DEVICEDATA_Video_settings="89"
DEVICEDATA_Audio_settings="88"
DEVICEDATA_Reboot="236"
DEVICEDATA_Connector="68"
DEVICEDATA_TV_Standard="229"
DEVICEDATA_Setup_Script="189"
DEVICEDATA_Sound_Card="288"
DEVICEDATA_Channel_Left="311"
DEVICEDATA_Channel_Right="312"
DEVICEDATA_Channel="81"
DEVICEDATA_Sampling_Rate="310"
DEVICEDATA_Distro_Raspbian_Wheezy="19"
DEVICEDATA_Distro_Raspbian_Jessie="22"

ARCH=$(arch)

SettingsFile="/etc/pluto/lastaudiovideo.conf"
mkdir -p $(dirname $SettingsFile)

# don't let KDE override xorg.conf
rm -f {/home/*,/root}/.kde/share/config/displayconfigrc

Reboot="NoReboot"
ReloadX="NoReloadX"
AudioSetting_Override="$1"
SoundCard_Override="$2"
XineConf_Override="$3"
ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')
OrbiterDev=$(FindDevice_Template "$ComputerDev" "$DEVICETEMPLATE_OnScreen_Orbiter")
VideoCardDev=$(FindDevice_Category "$ComputerDev" "$DEVICECATEGORY_Video_Cards")
AsoundConf="/usr/pluto/templates/asound.conf"
ConfGet "AlternateSC"

# FIXME: This needs to move to an rpi version of AVWizard, for now here is fine
case "$PK_Distro" in
	$DEVICEDATA_Distro_Raspbian_Wheezy|$DEVICEDATA_Distro_Raspbian_Jessie)
		# Ensure there is no xorg.conf on raspbian
		[ -f /etc/X11/xorg.conf ] && rm /etc/X11/xorg.conf

		# Detect and use the current boot resolution 
		FBWIDTH=$(for arg in $(cat /proc/cmdline); do echo $arg |grep "fbwidth" | cut -d '=' -f 2; done)
		FBHEIGHT=$(for arg in $(cat /proc/cmdline); do echo $arg |grep "fbheight" | cut -d '=' -f 2; done)

		Video_settings=$(GetDeviceData "$PK_Device" "$DEVICEDATA_Video_settings")
		if [[ "$Video_settings" != "$FBWIDTH $FBHEIGHT/60" ]]; then
			SetDeviceData "$PK_Device" "$DEVICEDATA_Video_settings" "$FBWIDTH $FBHEIGHT/60"

			# trigger an orbitergen
			Q="UPDATE Orbiter set Regen=1 where PK_Orbiter=$OrbiterDev"
			RunSQL "$Q"

			# FIXME: Grab these values from environment
			SetDeviceData "$PK_Device" "$DEVICEDATA_Connector" "HDMI-0"
			SetDeviceData "$PK_Device" "$DEVICEDATA_TV_Standard" "1080P"
		fi

		Audio_Settings=$(GetDeviceData "$PK_Device" "$DEVICEDATA_Audio_settings")
		rpioutput='2' # HDMI is default audio out
		if [[ "$Audio_Settings" == "S" ]]; then
			rpioutput='1'
		fi
		amixer cset numid=3 $rpioutput

		# Prevent /etc/asound.conf from being generated on rpi
		NoAsoundConf="true"
		;;
esac

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
		Refresh=$(echo "$VideoSetting" | cut -d '/' -f2)
		ResolutionInfo=$(echo "$VideoSetting" | cut -d '/' -f1)
		ResX=$(echo "$ResolutionInfo" | cut -d' ' -f1)
		ResY=$(echo "$ResolutionInfo" | cut -d' ' -f2)
		if [[ -z "$Refresh" || -z "$ResX" || -z "$ResY" ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "SetupAudioVideo.sh" "Malformed DeviceData: VideoSetting='${VideoSetting}'"
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

Setup_VirtualCards()
{
	local Q R
	mkdir -p /etc/pluto/alsa
	local Vfile=/etc/pluto/alsa/virtual_cards.conf
	local Devices=$(FindDevice_Template "$PK_Device" "$DEVICETEMPLATE_Stereo_virtual_sound_card" "" "" all)
	local Dev ParentDev SoundCard
	local Channel_Left Channel_Right
	local SampleRate IpcKey

	>"$Vfile"
	for Dev in $Devices; do
		Channel_Left=$(GetDeviceData "$Dev" "$DEVICEDATA_Channel_Left")
		Channel_Right=$(GetDeviceData "$Dev" "$DEVICEDATA_Channel_Right")
		Q="SELECT FK_Device_ControlledVia FROM Device WHERE PK_Device=$Dev"
		R=$(RunSQL "$Q")

		ParentDev=$(Field 1 "$R")
		SoundCard=$(GetDeviceData "$ParentDev" "$DEVICEDATA_Sound_Card")
		SoundCard=$(TranslateSoundCard "$SoundCard")

		if [[ -z "$SoundCard" ]]; then
			continue
		fi

		Channels=$(GetDeviceData "$ParentDev" "$DEVICEDATA_Channel")
		if [[ ! "$Channels" =~ ^[0-9]+$ ]]; then
			continue
		fi

		SampleRate=$(GetDeviceData "$ParentDev" "$DEVICEDATA_Sampling_Rate")
		if [[ -z "$SampleRate" ]]; then
			SampleRate=44100
		fi

		IpcKey=$(printf "0x%08x" $((0x72380000 + ParentDev)))
		cat >>"$Vfile" <<END
pcm.Virtual_$Dev {
	type plug
	slave.pcm {
		type dmix
		ipc_key $IpcKey
		bindings.0 $Channel_Left
		bindings.1 $Channel_Right
		slave {
			pcm "hw:$SoundCard,0,0"
			channels $Channels
			rate $SampleRate
		}
	}
}
END
	done
}

Enable_Audio_Channels() 
{
	# Added this to correctly unmute channels for setup wizard, and to 
	# inject necessary unmuting commands for later bootup.
	yalpa=$(aplay -l)
	grep -iwo "card ." <<< "$yalpa" | awk '{print $2}' | uniq | while read CardNumber; do
		amixer -c "$CardNumber" | grep '\[off\]' -B5 | grep "Simple" | sed 's/Simple mixer control //g' | grep -viE '(capture|mic)' | while read MuteStatus; do 
			amixer -c "$CardNumber" sset "$MuteStatus" unmute 
		done
		alsactl store
		amixer -c "$CardNumber" | grep '\[.*\%\]' -B5 | grep "Simple" | sed 's/Simple mixer control //g' | grep -viE '(capture|mic)' | while read VolLevel; do 
			amixer -c "$CardNumber" sset "$VolLevel" 80%
		done 2>&1>/dev/null
		alsactl store
	done
}

Setup_AsoundConf()
{
	[[ -n "$NoAsoundConf" ]] && return

	local AudioSetting="$1"
	SoundOut="plughw:"
	Yalpa=$(aplay -l 2>&1)
	# Do not mess with asound.conf if Audio Setting is set to Manual. This will only happen after the asound.conf has been generated at least once.
	if [[ "$AudioSetting" == "M" ]]; then
		return		
	fi
	# Do not mess with raspbian based systems
	if [[ "$PK_Distro" == "$DEVICEDATA_Distro_Raspbian_Wheezy" ]] || [[  "$PK_Distro" == "$DEVICEDATA_Distro_Raspbian_Jessie" ]] ; then
		return
	fi
	if grep 'no soundcards found' <<< "$Yalpa"; then
		return
	fi

	local Q R
	local MD_Device="$PK_Device"

	Setup_VirtualCards

	Q="SELECT FK_DeviceTemplate FROM Device WHERE PK_Device=$PK_Device"
	R=$(RunSQL "$Q")

	if [[ "$R" == "7" ]]; then
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
	HWOnlyCard="$SoundCard"
	# Handle nVidia GT card types
	case "$AudioSetting" in
		*[CO]*)
			CardDevice=$(grep -i "card" <<< "$Yalpa" | grep -v "device 0" | grep -vi "HDMI" | grep -wo "device ." | awk '{print $2}' | head -1)
			ConnectType="spdif"
			PlaybackPCM="spdif_playback"
			;; 
		*H*)
			ManID=$(grep "card $HWOnlyCard" <<< "$Yalpa" | grep -Eio '(nvidia|intel)' | head -1 |  tr '[:upper:]' '[:lower:]')

			CardDevice=$(grep -i "hdmi" <<< "$Yalpa" | grep -wo "device ." | awk '{print $2}')
				case "$ManID" in 
  					intel) 
						if [[ $(wc -l <<< "$CardDevice") -gt "1" ]]; then 
							ELDDevice=$(grep -l "eld_valid.*1" /proc/asound/card${SoundCard}/eld* | sort -u | head -1) 
								case "$ELDDevice" in 
									*.0) 
										CardDevice="3" ;; 
									*.1) 
										CardDevice="7" ;; 
									*.2) 
										CardDevice="8" ;; 
									*.3) 
										CardDevice="9" ;; 
									*) 
										CardDevice="3" ;; 
								esac 
						fi ;; 
					nvidia) 
						if [[ $(wc -l <<< "$CardDevice") -gt "3" ]]; then 
							ELDDevice=$(grep -l "eld_valid.*1" /proc/asound/card${SoundCard}/eld* | sort -u | head -1) 
								case "$ELDDevice" in 
								       *0.0) 
									       CardDevice="3" ;; 
									*1.0) 
										CardDevice="7" ;; 
									*2.0) 
										CardDevice="8" ;; 
									*3.0) 
										CardDevice="9" ;; 
									*) 
										CardDevice="7" ;; 
								esac 
						fi ;; 
				esac 

			ConnectType="hdmi"
			PlaybackPCM="hdmi_playback"
			;;
		*)
			CardDevice=$(grep -i "card" <<< "$Yalpa" | grep -i "Analog" | grep -wo "device ." | awk '{print $2}' | head -1)
			SoundOut="plug:dmix:"
			ConnectType="analog"
			PlaybackPCM="${SoundOut}${SoundCard}"
			;;
	esac

	if [[ "$AlternateSC" -ne "2" && "$AudioSetting" != "S" ]]; then
		SoundCard="${HWOnlyCard},${CardDevice}"
	fi

	if [[ "$AlternateSC" -gt "0" ]]; then
		AsoundConf="/usr/pluto/templates/asound.conf.backup"
	fi
	local PlaybackCard="${SoundOut}${SoundCard}"

	# Replace template values with choices
	sed -r "s#%CONNECT_TYPE%#${ConnectType}#g; s#%SOUND_OUT%#${SoundOut}#g; s#%MAIN_CARD%#${SoundCard}#g; s#%PLAYBACK_PCM%#${PlaybackPCM}#g;" "$AsoundConf" > /etc/asound.conf
	Setup_XineConf "$AudioSetting" "$PlaybackCard"
	Setup_LibVLCConf "$AudioSetting" "$PlaybackCard"

	alsa force-reload

	if pgrep AVWizard_Run.sh > /dev/null; then 
		Enable_Audio_Channels
	fi
}

Setup_LibVLCConf()
{
	local AudioSetting="$1" PlaybackCard="$2"

        if [ -f "/etc/pluto/libvlc.conf" ]; then 
		rm /etc/pluto/libvlc.conf
        fi
	
	touch /etc/pluto/libvlc.conf

	echo "--alsa-audio-device" >/etc/pluto/libvlc.conf
	echo "$PlaybackCard" >>/etc/pluto/libvlc.conf

	case "$AudioSetting" in
	*3*)
		echo "--spdif" >>/etc/pluto/libvlc.conf
		;;
	esac

}

Setup_XineConf()
{
	local AudioSetting="$1" PlaybackCard="$2"
	local XineConf=/etc/pluto/xine.conf
	sed -i '/audio\.device.*/d' "$XineConf"
	sed -i '/audio\.output.*/d' "$XineConf"

	if [[ -n "$XineConf_Override" ]]; then
		XineConf="$XineConf_Override"
	fi

	if [[ "$AlternateSC" -ne "1" ]]; then
		XineConfSet audio.device.alsa_front_device "$PlaybackCard" "$XineConf"
		XineConfSet audio.device.alsa_default_device "$PlaybackCard" "$XineConf"
		case "$AudioSetting" in
			*[COH]*)
				XineConfSet audio.device.alsa_pcm_device "$PlaybackCard" "$XineConf"

				XineConfSet audio.output.speaker_arrangement 'Pass Through' "$XineConf"
				XineConfSet audio.device.alsa_passthrough_device "$PlaybackCard" "$XineConf"

				XineConfSet audio.device.alsa_surround40_device "$PlaybackCard" "$XineConf"
				XineConfSet audio.device.alsa_surround41_device "$PlaybackCard" "$XineConf"
				XineConfSet audio.device.alsa_surround50_device "$PlaybackCard" "$XineConf"
				XineConfSet audio.device.alsa_surround51_device "$PlaybackCard" "$XineConf"
				;;
		esac
	else
		case "$AudioSetting" in
			*[COH]*)
				XineConfSet audio.device.alsa_passthrough_device "$PlaybackCard" "$XineConf"
				;;
			*)
				sed -i '/audio\.device\.alsa_passthrough_device.*/d' "$XineConf"
				;;
		esac
	fi

	case "$AudioSetting" in
		*3*)
			XineConfSet audio.output.speaker_arrangement 'Pass Through' "$XineConf"
			XineConfSet audio.device.alsa_passthrough_device "$PlaybackCard" "$XineConf"
			;;

		*)
			XineConfSet audio.output.speaker_arrangement 'Stereo 2.0' "$XineConf"
			sed -i '/audio\.device\.alsa_passthrough_device.*/d' "$XineConf"
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
		# Raspbian does not use Xconfigure
		if [[ "$PK_Distro" != "$DEVICEDATA_Distro_Raspbian_Wheezy" ]] && [[ "$PK_Distro" != "DEVICEDATA_Distro_Raspbian_Jessie" ]] ; then
			/usr/pluto/bin/Xconfigure.sh
		fi

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

	if [[ -z "$AlternateSC" ]]; then
		AlternateSC="0"
	fi

	Setup_AsoundConf "$NewSetting_AudioSetting" "$AlternateSC"

	if [[ "$DB_Reboot" == "1" ]]; then
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

if [[ "$Reboot" == "Reboot" ]]; then
	reboot
elif [[ "$ReloadX" == "ReloadX" ]]; then
	/usr/pluto/bin/RestartLocalX.sh &
	disown -a
fi
