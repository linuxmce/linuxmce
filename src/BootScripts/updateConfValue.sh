#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

ConfSet "$2" "$3"
#sed -i -e "s/$2.*/$2=$3/" "$1"
