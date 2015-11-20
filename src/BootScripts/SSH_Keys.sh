#!/bin/bash

. /usr/pluto/bin/Utils.sh

Dir=/usr/pluto/keys
Key=id_dsa_pluto

if [ ! -f /etc/diskless.conf ] && ! [ -f $Dir/$Key -a -f $Dir/$Key.pub ]; then
	echo "Creating SSH keys"
	mkdir -p /usr/pluto/keys
	ssh-keygen -t dsa -C "Auto-generated key for MD to talk to Core" -f $Dir/$Key -P ""
	cp $Dir/$Key $Dir/${Key}_apache
	chown www-data.www-data $Dir/${Key}_apache
fi

mkdir -m 0700 -p /root/.ssh

if [ ! -e /etc/ssh/ssh_config.pbackup ] && [ -e /etc/ssh/ssh_config ] ;then
	cp /etc/ssh/ssh_config /etc/ssh/ssh_config.pbackup
fi

if [ ! -e /root/.ssh/config.pbackup ] ;then
	cp /root/.ssh/config /root/.ssh/config.pbackup
fi

if ! grep -q "^ *StrictHostKeyChecking no *$" /etc/ssh/ssh_config ;then
	echo "Writing defaults to ssh_config"
	cp /usr/pluto/templates/ssh_config.tmpl /etc/ssh/ssh_config
fi

if ! grep -q "^ *UseDNS no *$" /etc/ssh/sshd_config ;then
	echo "UseDNS no" >> /etc/ssh/sshd_config
fi

for i in /etc/ssh/ssh_config /root/.ssh/config; do
	grep 'Host..*\*' "$i" &>/dev/null && HostFound=1
done

File='/root/.ssh/config'
if ! BlacklistConfFiles "$File" ;then
	if [ -z "$HostFound" ] || ! grep "IdentityFile $Dir/$Key" "$File" &>/dev/null; then
		HostEntry="Host *
		IdentityFile $Dir/$Key"
		echo "$HostEntry" >>"$File"
	fi
fi

KeyString="$(cat $Dir/$Key.pub)"
File='/root/.ssh/authorized_keys'
[ -f "$File" ] && grep -F "$KeyString" "$File" &>/dev/null && KeyPresent=1

[ -z "$KeyPresent" ] && cat $Dir/$Key.pub >>"$File"

service ssh restart
