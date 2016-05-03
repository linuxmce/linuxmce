#!/bin/sh

##
## Setup Script for the Phoenix Solo USB microphone && Duet USB speaker/microphone.
##
## TODO: Store volume values in Device's DD
##

## MDs AudioSettings parameter isn't used at all. This is a
## completely separate device from the main sound in an MD.

CARDID=$(aplay -l | grep Phnx | awk '{ print $2 }' | cut -d':' -f1)

amixer -c "$CARDID" sset "PCM",0 50 >/dev/null
amixer -c "$CARDID" sset "Speaker",0 44 >/dev/null
amixer -c "$CARDID" sset "Mic",0 50 >/dev/null

