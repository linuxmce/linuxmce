#!/bin/bash

# Hack to add a template
if ! grep -q '^Name: shared/accepted-sun-dlj-v1-1' /var/cache/debconf/config.dat ;then
	echo 'Name: shared/accepted-sun-dlj-v1-1' >> /var/cache/debconf/config.dat
	echo 'Template: shared/accepted-sun-dlj-v1-1' >> /var/cache/debconf/config.dat
	echo 'Owners: sun-java6-bin, sun-java6-jre' >>  /var/cache/debconf/config.dat
fi
#Seeding mythweb preferences to not override the LMCE site on install
touch /etc/default/mythweb
echo "[cfg]" >> /etc/default/mythweb
echo "enable = false" >> /etc/default/mythweb
echo "only = false" >> /etc/default/mythweb
echo "username = " >> /etc/default/mythweb
echo "password = " >> /etc/default/mythweb 

. /usr/share/debconf/confmodule

db_version 2.0
db_capb backup


## Set deb-frontend to noninteractive
db_set "debconf/frontend" "Noninteractive"
db_fset "debconf/frontend" seen true
db_set "debconf/priority" "critical"
db_fset "debconf/priority" seen true

## Set the ivtv-firmware license as 'accepted'
db_fset "ivtv/present-hauppauge-eu-v1" seen true
db_set "ivtv/accepted-hauppauge-eu-v1" 'true'
db_fset "ivtv/accepted-hauppauge-eu-v1" seen true

## Set the sun-java license as 'accepted'
db_register 'dash' 'shared/present-sun-dlj-v1-1' 
db_register 'dash' 'shared/error-sun-dlj-v1-1'
db_register 'dash' 'shared/accepted-sun-dlj-v1-1'
db_set "shared/accepted-sun-dlj-v1-1" 'true' || echo "BAG P"
db_fset "shared/present-sun-dlj-v1-1" seen true
db_fset "shared/error-sun-dlj-v1-1" seen true
db_fset "shared/accepted-sun-dlj-v1-1" seen true

## Prevent mythweb from taking over web admin
db_set "mythweb/only" 'false'
db_fset "mythweb/only" seen true

## Set the ttf-mscorefonts-installer EULA as 'accepted'
db_set "msttcorefonts/accepted-mscorefonts-eula" 'true'
db_fset "msttcorefonts/accepted-mscorefonts-eula" seen true
db_set "msttcorefonts/present-mscorefonts-eula" 'false'
db_fset "msttcorefonts/present-mscorefonts-eula" seen true
