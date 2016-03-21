#!/usr/bin/expect

set timeout 1
set email [lindex $argv 0]
set password [lindex $argv 1]
set name [lindex $argv 2]
set country [lindex $argv 3]
set state [lindex $argv 4]
set org [lindex $argv 5]
set dn [lindex $argv 6]

spawn openssl genrsa -des3 -out key.pem 1024
expect -re "Enter pass phrase for key.pem:"
send "$password\n"
expect -re "Verifying - Enter pass phrase for key.pem:"
send "$password\n"
expect -re "writing RSA key"

spawn openssl req -new -key key.pem -out req.pem
expect -re "Enter pass phrase for key.pem:"
send "$password\r"
expect -re "Country Name (2 letter code)"
send "$country\r"
expect -re "State or Province Name (full name)"
send "$state\r"
expect -re "Locality Name (eg, city)"
send ".\r"
expect -re "Organization Name (eg, company)"
send "$org\r"
expect -re "Organizational Unit Name"
send ".\r"
expect -re "Common Name (eg, your name or your server's hostname)"
send "$name\r"
expect -re "Email Address "
send "$email\r"
expect -re "A challenge password"
send "\r"
expect -re "An optional company name"
send "\r"

spawn mv key.pem key.pem.orig

spawn openssl rsa -in key.pem.orig -out key.pem
expect "Enter pass phrase for key.pem.orig:"
send "$password\r"
expect "writing RSA key"

spawn openssl x509 -req -days 365 -in req.pem -signkey key.pem -out cert.pem
expect -re "Getting Private key"

interact
sleep 1
