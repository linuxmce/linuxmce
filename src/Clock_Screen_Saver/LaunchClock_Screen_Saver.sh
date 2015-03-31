#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh

export SDL_VIDEO_X11_WMCLASS="Clock_Screen_Saver"
export DISPLAY=:$Display

Executable=./Clock_Screen_Saver

exec "$Executable" "$@"
