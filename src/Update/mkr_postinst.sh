#!/bin/bash

#called from init.d/linuxmce now
#update-rc.d -f LMCEUpdate remove >/dev/null
#update-rc.d -f LMCEUpdate start 98 2 3 4 5 . >/dev/null
if [[ -e /etc/init.d/LMCEUpdate ]]; then
        update-rc.d -f LMCEUpdate remove >/dev/null
        rm -f /etc/init.d/LCMEUpdate
fi

