#!/bin/bash

if [ install = "$1" ]; then
	if [[ ! $(grep "PK_Device" /etc/pluto.conf) ]] ; then
		#StatsMessage "Setting initial PK_Device"
		cat <<-EOF >/etc/pluto.conf
			PK_Device = 1
			EOF
		chmod 777 /etc/pluto.conf &>/dev/null
	fi

	if [ $(/bin/hostname) != "dcerouter" ] ; then
		## Setup /etc/hosts
		cat <<-EOF >/etc/hosts
			127.0.0.1 localhost.localdomain localhost
			127.0.1.1 dcerouter $(/bin/hostname)
			EOF
	fi

#	# Put a temporary nis config file that will prevent ypbind to start
#	# Temporary NIS setup, disabling NIS server and client.
#	if [ ! -f /etc/default/nis ] ; then
#		cat <<-EOF >/etc/default/nis
#			NISSERVER=false
#			NISCLIENT=false
#			YPPWDDIR=/etc
#			YPCHANGEOK=chsh
#			NISMASTER=
#			YPSERVARGS=
#			YPBINDARGS=
#			YPPASSWDDARGS=
#			YPXFRDARGS=
#			EOF
#	fi

	if [ ! -f /etc/bind/named.conf.options ] ; then
		cat <<-EOF >/etc/bind/named.conf.options
			options {
				directory "/var/cache/bind";

				// If there is a firewall between you and nameservers you want
				// to talk to, you may need to fix the firewall to allow multiple
				// ports to talk.  See http://www.kb.cert.org/vuls/id/800113

				// If your ISP provided one or more IP addresses for stable
				// nameservers, you probably want to use them as forwarders.
				// Uncomment the following block, and insert the addresses replacing
				// the all-0's placeholder.

				// forwarders {
				// 	0.0.0.0;
				// };

				//========================================================================
				// If BIND logs error messages about the root key being expired,
				// you will need to update your keys.  See https://www.isc.org/bind-keys
				//========================================================================
				dnssec-validation no;

				auth-nxdomain no;    # conform to RFC1035
				listen-on-v6 { any; };
			};
			EOF
	fi
fi

exit 0
