#!/bin/sh /etc/rc.common

START=3

start() {
	nvram get backup | uudecode > /tmp/bk2nvram.tar.gz
	cd /
	tar zxvf /tmp/bk2nvram.tar.gz

#	rm /tmp/backup.tar.gz

	nvram set backup=""
	nvram commit
}

stop() {
	:
}

