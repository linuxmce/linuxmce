#!/bin/bash
## Create divertion for SaveMythRecordings, so we don't get a problem on older installs

if [ install = "$1" -o upgrade = "$1" ]; then
        dpkg-divert --add --rename --package lmce-mythtv-scripts --divert /usr/pluto/bin/SaveMythRecording.sh.wraped /usr/pluto/bin/SaveMythRecording.sh
fi
                
                