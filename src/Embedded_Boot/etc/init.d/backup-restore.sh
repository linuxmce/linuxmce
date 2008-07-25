#!/bin/sh /etc/rc.common

START=3

start() {
	nvram get backup | uudecode > /tmp/backup.tar.gz
	cd /
	tar zxvf /tmp/backup.tar.gz

	rm /tmp/backup.tar.gz

	nvram set backup=""
	nvram commit
}

stop() {
}
