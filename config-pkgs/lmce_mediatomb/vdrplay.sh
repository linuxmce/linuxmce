#!/bin/bash
. $1
OTHERS=`ps xa | grep $URL | grep wget | awk '{ print $1 }'`
[ "$OTHERS" == "" ] || kill -9 $OTHERS
wget -q -O $2 $URL
