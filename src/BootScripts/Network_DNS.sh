#!/bin/bash

. /usr/pluto/bin/Utils.sh

if [ ! -e /etc/resolv.conf.pbackup ] ;then
	cp /etc/resolv.conf /etc/resolv.conf.pbackup
fi

if [ ! -e /etc/bind/named.conf.forwarders.pbackup ] && [ -e /etc/bind/named.conf.forwarders ] ;then
	cp /etc/bind/named.conf.forwarders /etc/bind/named.conf.forwarders.pbackup
fi	

if ! BlacklistConfFiles '/etc/resolv.conf' && ! BlacklistConfFiles '/etc/bind/named.conf.forwarders' ;then

Parm="$1"

cat >/etc/bind/named.conf.forwarders <<END
forwarders {
$(grep nameserver /etc/resolv.conf | grep -v '#' | sed 's/nameserver//g; s/ *//g; s/^.*$/\t&;/')
};
END
fi
## For some reason, 'rndc reload' didn't refresh the forwarders for at least one occasion
## Because of this, the default is to restart BIND.
## Only special callers will reload BIND instead of restarting it.
case "$1" in
	dhclienthook)
		rndc reload
	;;

	*)
		service bind9 restart
	;;
esac
