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

# reinstall packages, to be honest: I don't know why. It was like this before i touched.
# Will investigate if be can drop this.
#apt-get -y remove postfix 
	 
#apt-get -y install postfix 
#apt-get -y install expect 
#apt-get -y install heirloom-mailx nail 
#apt-get -y install ca-certificates 
	 
if [ -e main.cf.orig ]; then 
	cp main.cf.orig main.cf 
else 
	cp main.cf main.cf.orig 
fi 

# Turn off history mode to allow ! in passwords
set +H

# If TLS is used generate certificates and configure postfix to use them
if [ "$tls" == "yes" ]; then 
	rm -rf demoCA 
	rm -f *.pem 
	rm -f *.pm 
	/usr/pluto/bin/Configure_Postfix_TLS.sh "$email" "$password" "$name" "$country" "$state" "$org" "/CN=dcerouter/C=$country/ST=$state/O=$org/CN=$name/emailAddress=$email" 
	echo "## TLS Settings" >> main.cf 
	echo "smtp_tls_loglevel = 1" >> main.cf 
	echo "smtp_enforce_tls = yes" >> main.cf 
	echo "smtp_tls_CAfile = /etc/postfix/cacert.pem" >> main.cf 
	echo "smtp_tls_cert_file = /etc/postfix/cert.pem" >> main.cf 
	echo "smtp_tls_key_file = /etc/postfix/key.pem" >> main.cf 
	echo "smtp_tls_session_cache_database = btree:/var/run/smtp_tls_session_cache" >> main.cf 
	echo "smtp_use_tls = yes" >> main.cf 
	echo "smtpd_tls_CAfile = /etc/postfix/cacert.pem" >> main.cf 
	echo "smtpd_tls_cert_file = /etc/postfix/cert.pem" >> main.cf 
	echo "smtpd_tls_key_file = /etc/postfix/key.pem" >> main.cf 
	echo "smtpd_tls_received_header = yes" >> main.cf 
	echo "smtpd_tls_session_cache_database = btree:/var/run/smtpd_tls_session_cache" >> main.cf 
	echo "smtpd_use_tls = yes" >> main.cf 
	echo "tls_random_source = dev:/dev/urandom" >> main.cf 
fi

# Generate new postfix config files
echo "dcerouter       relay:[dcerouter]" >transport 
echo "*               smtp:[$mta]:$port" >>transport 
/usr/sbin/postmap transport &> /tmp/transportmap
	 
echo "[$mta]:$port             $username:$password" >sasl_passwd 
chown root:root /etc/postfix/sasl_passwd && chmod 600 /etc/postfix/sasl_passwd
/usr/sbin/postmap /etc/postfix/sasl_passwd
	 
rm -f generic ; touch generic 
/usr/sbin/postmap generic 
	 
echo "" >> main.cf 
echo "##  SASL Settings" >> main.cf 
echo "# This is going in to THIS server" >> main.cf 
echo "smtpd_sasl_auth_enable = no" >> main.cf 
echo "# We need this" >> main.cf 
echo "smtp_sasl_auth_enable = yes" >> main.cf 
echo "smtp_sasl_password_maps = hash:/etc/postfix/sasl_passwd" >> main.cf 
echo "smtpd_sasl_local_domain = $myhostname" >> main.cf 
echo "smtp_sasl_security_options = noanonymous" >> main.cf 
echo "#smtp_sasl_security_options =" >> main.cf 
echo "smtp_sasl_tls_security_options = noanonymous" >> main.cf 
echo "#smtpd_sasl_application_name = smtpd" >> main.cf 
echo "" >> main.cf 
echo "relayhost = [$mta]:$port" >> main.cf 
echo "disable_dns_lookups = yes" >> main.cf 
echo "smtp_generic_maps = hash:/etc/postfix/generic" >> main.cf 
echo "transport_maps = hash:/etc/postfix/transport" >> main.cf 

#cp demoCA/cacert.pem . 
#chmod 644 cert.pem  
#chmod 644 cacert.pem 
#chmod 400 key.pem
 
# if [ "$emailservice" == "gmail" ]; then
if [ "$tls" == "yes" ]; then 
	cat /etc/ssl/certs/Equifax_Secure_CA.pem >> /etc/postfix/cacert.pem
fi

# reload new config
service postfix reload
exit 0
