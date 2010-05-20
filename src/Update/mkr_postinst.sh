#!/bin/bash

update-rc.d LMCEUpdate start 98 2 . > /dev/null

# Check if we have a pinning to origin deb.linuxmce.org, and if not add it.

EXIST=0
grep -q "Pin: origin deb.linuxmce.org" /etc/apt/preferences 2>/dev/null && EXIST=1

if [[ "$EXIST" == "0" ]]; then
   echo "Package: *">> /etc/apt/preferences
   echo "Pin: origin deb.linuxmce.org">>/etc/apt/preferences
   echo "Pin-Priority: 1001">>/etc/apt/preferences
   echo "Pinning setup for deb.linuxmce.org"
else
   echo "Pinning for deb.linuxmce.org already exists"
fi
