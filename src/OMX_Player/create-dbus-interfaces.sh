#!/bin/sh

dbusxx-xml2cpp omxplayer-dbus-glue.xml --proxy=omxplayer-dbus-glue.h
sed -i 's/Propertier/Properties/g' ./omxplayer-dbus-glue.h
