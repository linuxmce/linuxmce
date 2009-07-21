#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Utils.sh

Param="$1"
case "$Param" in
	'Analog Stereo')
		XineConfSet audio.output.speaker_arrangement 'Stereo 2.0' "$XineConf"
		XineConfSet audio.device.alsa_front_device plughw:0 "$XineConf"
		XineConfSet audio.device.alsa_default_device plughw:0 "$XineConf"
	;;
	'SPDIF Coaxial'|'SPDIF Optical')
		XineConfSet audio.output.speaker_arrangement 'Pass Through' "$XineConf"
		XineConfSet audio.device.alsa_front_device asym_spdif "$XineConf"
		XineConfSet audio.device.alsa_default_device asym_spdif "$XineConf"
		XineConfSet audio.device.alsa_passthrough_device "iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2" "$XineConf"
	;;
	'HDMI')
		XineConfSet audio.output.speaker_arrangement 'Pass Through' "$XineConf"
		XineConfSet audio.device.alsa_front_device asym_hdmi "$XineConf"
		XineConfSet audio.device.alsa_default_device asym_hdmi "$XineConf"
		XineConfSet audio.device.alsa_passthrough_device "hdmi:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2" "$XineConf"
	;;
esac
/usr/pluto/bin/SetupAudioVideo.sh
