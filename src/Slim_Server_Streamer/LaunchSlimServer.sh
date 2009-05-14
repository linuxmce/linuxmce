#!/bin/bash

PORT="${1:-9090}"
PARAMS="--cliport $PORT --audiodir /home/public/data/audio"

sed -ri "s,SLIMOPTIONS=.*,SLIMOPTIONS=\"$PARAMS\",g" /etc/default/squeezecenter

invoke-rc.d squeezecenter restart
