#!/bin/dash
mkdir -p /usr/pluto/diversions
dpkg-divert --rename --remove /etc/init.d/squeezelite
