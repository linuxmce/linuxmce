#!/bin/bash
## Create divertion for NetworkManager as it destroys our settings

if [ install = "$1" -o upgrade = "$1" ]; then
        dpkg-divert --add --rename --package pluto-boot-scripts --divert /usr/sbin/NetworkManager.wraped /usr/sbin/NetworkManager
fi
                
                