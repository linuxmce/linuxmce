#!/bin/bash
## Create divertion for pluto's EIB plugin

if [ install = "$1" -o upgrade = "$1" ]; then
        dpkg-divert --add --rename --package lmce-eib-eibd --divert /usr/pluto/bin/EIB.pluto-eib /usr/pluto/bin/EIB
fi
                
                