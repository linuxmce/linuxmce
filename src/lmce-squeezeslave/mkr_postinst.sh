#!/bin/dash
mkdir -p /usr/pluto/diversions
dpkg-divert --divert /usr/pluto/diversions/squeezelite --rename /etc/init.d/squeezelite
