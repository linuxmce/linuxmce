#!/bin/bash

DevEntry="$1" #/dev/videoX
VideoInput="$2"
AudioInput="$3" #optional

ModuleDefaultAudio=/sys/module/hdpvr/parameters/default_audio_input
v4lctl=v4l2-ctl
#v4lctl=echo

# hardcoded input values as they are in the database
# also, positions in the Inputs array

Video_Component=0
Video_SVideo=1
Video_Composite=2
VideoInputs=($Video_Component $Video_SVideo $Video_Composite)

Audio_RCABack=0
Audio_RCAFront=1
Audio_SPDIF=2
AudioInputs=($Audio_RCABack $Audio_RCAFront $Audio_SPDIF)

if [ -z "${VideoInputs[$VideoInput]}" ]; then
    echo Invalid video input: $VideoInput
    exit 1
fi

$v4lctl --device="$DevEntry" --set-input="${VideoInputs[$VideoInput]}"
rc=$?
if [ "$rc" != "0" ]; then
    echo "Error: $v4lctl returned: $rc"
    exit $rc
fi

if [ -n "$AudioInput" ]; then
    if [ -z "${AudioInputs[$AudioInput]}" ]; then
        echo Invalid audio input: $AudioInput
        exit 1
    fi

    if [ "${AudioInputs[$AudioInput]}" == "$Audio_SPDIF" -a -f $ModuleDefaultAudio ]; then
      echo Setting default audio input to $Audio_SPDIF to work around bug with firmware
      echo $Audio_SPDIF > $ModuleDefaultAudio
    fi

    $v4lctl --device="$DevEntry" --set-audio-input="${AudioInputs[$AudioInput]}"
    rc=$?
    if [ "$rc" != "0" ]; then
        echo "Error: $v4lctl returned: $rc"
        exit $rc
    fi
fi
