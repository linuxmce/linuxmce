#!/bin/bash
## Create divertion for NetworkManager as it destroys our settings

if [ install = "$1" -o upgrade = "$1" ]; then
        dpkg-divert --add --rename --package pluto-boot-scripts --divert /usr/sbin/NetworkManager.wraped /usr/sbin/NetworkManager
fi

if [ install = "$1" ]; then
	cat <<-EOF >/tmp/preseed.cfg
		debconf	debconf/frontend	select	Noninteractive
		# Choices: critical, high, medium, low
		debconf	debconf/priority	select	critical
		msttcorefonts	msttcorefonts/http_proxy	string
		msttcorefonts	msttcorefonts/defoma	note
		msttcorefonts	msttcorefonts/dlurl	string
		msttcorefonts	msttcorefonts/savedir	string
		msttcorefonts	msttcorefonts/baddldir	note
		msttcorefonts	msttcorefonts/dldir	string
		msttcorefonts	msttcorefonts/blurb	note
		msttcorefonts	msttcorefonts/accepted-mscorefonts-eula	boolean	true
		msttcorefonts	msttcorefonts/present-mscorefonts-eula	boolean	false
		sun-java6-bin	shared/accepted-sun-dlj-v1-1	boolean	true
		sun-java6-jre	shared/accepted-sun-dlj-v1-1	boolean	true
		sun-java6-jre	sun-java6-jre/jcepolicy	note
		sun-java6-jre	sun-java6-jre/stopthread	boolean	true
		EOF
	debconf-set-selections /tmp/preseed.cfg
fi
