#!/bin/bash 
 
apt-get -y install postfix 
apt-get -y install expect 
apt-get -y install mailx 
apt-get -y install ca-certificates 
 
email="$1" 
password="$2" 
name="$3" 
mta=$4 
country=$5 
state=$6 
org=LinuxMCE 
tls=$7 
port=$8 
	 
if [ $mta = "gmail" ] 
then 
port=587 
mta=smtp.gmail.com 
tls=tls 
fi  
	 
cd /etc/postfix 
	 
if [ -e main.cf.orig ] 
then 
cp main.cf.orig main.cf 
else 
cp main.cf main.cf.orig 
fi 
	 
if [ $tls = "tls" ] 
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
	 
cat /etc/ssl/certs/Thawte_Premium_Server_CA.pem >> cacert.pem  
 	 
service postfix stop || :
service postfix start
	 
sleep 10 
	 
echo "sending test email..." 
mailx -s "testing from linuxmce" $email < /etc/hosts 
	 
sleep 10 
 	 
tail /var/log/mail.info | grep status= 
