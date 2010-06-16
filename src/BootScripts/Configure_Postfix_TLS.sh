#!/usr/bin/expect 
 
set email [lindex $argv 0] 
set password [lindex $argv 1] 
set name [lindex $argv 2] 
set country [lindex $argv 3] 
set state [lindex $argv 4] 
set org [lindex $argv 5] 
set dn [lindex $argv 6] 
	 
spawn /usr/lib/ssl/misc/CA.pl -newca 
expect "CA certificate filename (or enter to create)" 
send "\r" 
expect "Enter PEM pass phrase:" 
send "$password\r" 
expect "Verifying - Enter PEM pass phrase:" 
send "$password\r" 
expect -re "Country Name" 
send "$country\r" 
expect -re "State or Province Name" 
send "$state\r" 
expect -re "Locality Name" 
send "\r" 
expect -re "Organization Name" 
send "$org\r" 
expect -re "Organizational Unit Name" 
send "\r" 
expect -re "Common Name" 
send "$name\r" 
expect -re "Email Address" 
send "$email\r" 
expect -re "A challenge password" 
send "\r" 
expect -re "An optional company name" 
send "\r" 
expect -re "Enter pass phrase" 
send "$password\r" 
expect "Data Base Updated" 
	 
spawn openssl x509 -in demoCA/cacert.pem -days 3650 -out cacert.pem -signkey demoCA/private/cakey.pem 
expect -re "Enter pass phrase" 
send "$password\r" 
	 
spawn cp cacert.pem demoCA 
	 
spawn openssl req -new -nodes -subj "$dn" -keyout key.pem -out req.pem -days 3650 
expect "nothing" 
	 
spawn openssl ca -out cert.pem -infiles req.pem 
expect -re "Enter pass phrase" 
send "$password\r" 
expect -re "Sign the certificate" 
send "y\r" 
expect -re "1 out of 1 certificate requests certified, commit" 
send "y\r" 
expect "Data Base Updated" 
 
exit 0 
