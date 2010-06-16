#!/bin/bash 
	 
name=$1 
email=$2 
password=$3 
mta=$4 
port=$5 
tls=$6 
org=$7 
state=$8 
country=$9 
shift 1
emailservice=$9

cd /etc/postfix 
	 
apt-get -y remove postfix 
	 
apt-get -y install postfix 
apt-get -y install expect 
apt-get -y install mailx 
apt-get -y install ca-certificates 
	 
if [ -e main.cf.orig ] 
then 
cp main.cf.orig main.cf 
else 
cp main.cf main.cf.orig 
fi 
	 
if [ $tls = "yes" ] 
then 
rm -rf demoCA 
rm *.pem 
rm *.pm 
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
	 
echo "dcerouter       relay:[dcerouter]" >transport 
echo "*               smtp:[$mta]:$port" >>transport 
postmap transport 
	 
echo "[$mta]:$port             $email:$password" >sasl_passwd 
postmap sasl_passwd 
	 
rm -f generic ; touch generic 
postmap generic 
	 
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
echo "smtpd_sasl_application_name = smtpd" >> main.cf 
echo "" >> main.cf 
echo "relayhost = [$mta]:$port" >> main.cf 
echo "disable_dns_lookups = yes" >> main.cf 
echo "smtp_generic_maps = hash:/etc/postfix/generic" >> main.cf 
echo "transport_maps = hash:/etc/postfix/transport" >> main.cf 
	 
cp demoCA/cacert.pem . 
chmod 644 cert.pem  
chmod 644 cacert.pem 
chmod 400 key.pem
 
/etc/init.d/postfix restart 
	 
sleep 10
	
if [ $emailservice = "gmail" ]
then
cat /etc/ssl/certs/Equifax_Secure_CA.pem >> /etc/postfix/cacert.pem
fi

sleep 5
 
echo "sending test email..." 
mailx -s "testing from linuxmcenew" $email < /etc/hosts 
	 
sleep 10 
	 
tail /var/log/mail.info | grep status= 
