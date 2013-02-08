#!/bin/bash

. /usr/pluto/bin/AVWizard-Common.sh
. /usr/pluto/bin/Utils.sh

AudioOption="$1"
SoundCard="$2"

AudioSetting=
case "$AudioOption" in
	'Analog Stereo')
		AudioSetting=S
	;;
	'SPDIF Coaxial'|'SPDIF Optical')
		AudioSetting=O3
	;;
	'HDMI')
		AudioSetting=H3
	;;
esac
/usr/pluto/bin/SetupAudioVideo.sh "$AudioSetting" "$SoundCard" /etc/pluto/xine.conf.avwizard
