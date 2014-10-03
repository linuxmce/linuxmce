#!/bin/dash
mkdir -p /usr/pluto/diversions
dpkg-divert --divert /usr/pluto/diversions/squeezelite.distrib/etc/init.d/squeezelite --rename /etc/init.d/squeezelite
