#!/bin/bash

update-rc.d LMCEUpdate start 98 2 . > /dev/null

# Check if we have a pinning to origin deb.linuxmce.org, and if not add it.

KEEPOLD=0
grep -q "# Dont overwrite" /etc/apt/preferences 2>/dev/null && KEEPOLD

if [[ "$KEEPOLD" == "0" ]]; then
   echo "Package: mythtv mythtv-frontend mythtv-backend mythtv-common mythtv-database mythtv-transcode-utils mythweb libmythtv-perl libmyth python-myth mythtv-themes mythplugins mytharchive mytharchive-data mythbrowser mythgallery mythgame mythmovies mythmusic mythnetvison mythnetvision-data mythnews mythvideo mythweather mythzoneminder myth-doc">> /etc/apt/preferences
   echo "Pin: origin deb.linuxmce.org">>/etc/apt/preferences
   echo "Pin-Priority: 1001">>/etc/apt/preferences
   echo "Pinning setup for deb.linuxmce.org"
fi
