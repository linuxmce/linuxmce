#!/bin/bash

name=$1
email=$2
username=$3

password=$4
mta=$5
port=$6
tls=$7
org=$8
state=$9
shift 2
country=$8
emailservice=$9

cd /etc/postfix

## FIXME: Check BlacklistConfFile
if [ -e main.cf.orig ]; then
	cp main.cf.orig main.cf
else
	cp main.cf main.cf.orig
fi

# Turn off history mode to allow ! in passwords
set +H

# If TLS is used generate certificates and configure postfix to use them
if [ "$tls" == "yes" ]; then
	sed "s/^smtp_tls_cert_file/#smtp_tls_cert_file/g" -i main.cf
	sed "s/^smtp_tls_key_file/#smtp_tls_key_file/g" -i main.cf
	sed "s/^smtpd_tls_cert_file/#smtp_tls_cert_file/g" -i main.cf
	sed "s/^smtpd_tls_key_file/#smtp_tls_key_file/g" -i main.cf

	rm -rf demoCA
	rm -f *.pem
	rm -f *.pm

	/usr/pluto/bin/Configure_Postfix_TLS.sh "$email" "$password" "$name" "$country" "$state" "$org" "/CN=dcerouter/C=$country/ST=$state/O=$org/CN=$name/emailAddress=$email"

	cat <<-EOF >>main.cf
		## TLS Settings" >> main.cf
		smtp_tls_loglevel = 1
		smtp_enforce_tls = yes

		smtp_tls_CAfile = /etc/postfix/cacert.pem
		smtp_tls_cert_file = /etc/postfix/cert.pem
		smtp_tls_key_file = /etc/postfix/key.pem
		#smtp_tls_session_cache_database = btree:/var/run/smtp_tls_session_cache
		smtp_use_tls = yes

		smtpd_tls_CAfile = /etc/postfix/cacert.pem
		smtpd_tls_cert_file = /etc/postfix/cert.pem
		smtpd_tls_key_file = /etc/postfix/key.pem
		smtpd_tls_received_header = yes
		#smtpd_tls_session_cache_database = btree:/var/run/smtpd_tls_session_cache
		smtpd_use_tls = yes

		tls_random_source = dev:/dev/urandom

		EOF
fi

# Generate new postfix config files
cat <<-EOF >transport
	dcerouter       relay:[dcerouter]
	*               smtp:[$mta]:$port
	EOF
/usr/sbin/postmap transport &> /tmp/transportmap
rm -f generic ; touch generic
/usr/sbin/postmap generic

cat <<-EOF >>main.cf
	relayhost = [$mta]:$port
	disable_dns_lookups = yes
	smtp_generic_maps = hash:/etc/postfix/generic
	transport_maps = hash:/etc/postfix/transport
	EOF

# Setup SASL Settings
cat <<-EOF >sasl_passwd
	[$mta]:$port             $username:$password
	EOF
chown root:root /etc/postfix/sasl_passwd && chmod 600 /etc/postfix/sasl_passwd
/usr/sbin/postmap /etc/postfix/sasl_passwd

cat <<-EOF >>main.cf
	##  SASL Settings
	# This is going in to THIS server
	smtpd_sasl_auth_enable = no

	# We need this
	smtp_sasl_auth_enable = yes
	smtp_sasl_password_maps = hash:/etc/postfix/sasl_passwd
	smtp_sasl_security_options = noanonymous
	smtp_sasl_tls_security_options = noanonymous

	smtpd_sasl_local_domain = $myhostname
	EOF

# if [ "$emailservice" == "gmail" ]; then
if [ "$tls" == "yes" ]; then
	#cat /etc/ssl/certs/Equifax_Secure_CA.pem >> /etc/postfix/cacert.pem  # ??
	cat /etc/ssl/certs/GlobalSign_Root_CA.pem >> /etc/postfix/cacert.pem  # hotmail
fi

# reload new config
service postfix reload
exit 0
