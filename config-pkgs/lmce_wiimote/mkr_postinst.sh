#!/bin/bash
###########

# The initial post-install happens here, start the wminput with default options so that
# it can be used to select other options.

screen -d -m -S "WMInput Discovery" /usr/pluto/bin/DiscoverWiiMote.sh 

