#!/bin/bash
## Create divertion for config.xml, as we have our own configuration file.

if [ install = "$1" -o upgrade = "$1" ]; then
        dpkg-divert --add --rename --package lmce-mediatomb --divert /etc/mediatomb/config.xml.wrapped /etc/mediatomb/config.xml
fi
