#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

#enable asterisk daemon
sed -r -i "s/RUNASTERISK=no/RUNASTERISK=yes/" /etc/default/asterisk
sed -r -i "s/[#]RUNASTSAFE=yes/RUNASTSAFE=yes/" /etc/default/asterisk

# Create some files that being included from other config files, but don't exist.
# 
#touch /etc/asterisk/manager_custom.conf
#touch /etc/asterisk/queues_custom.conf
#touch /etc/asterisk/sip_custom.conf
#touch /etc/asterisk/sip_registrations.conf
#touch /etc/asterisk/sip_registrations_custom.conf
#touch /etc/asterisk/sip_nat.conf
#touch /etc/asterisk/sip_general_custom.conf
#touch /etc/asterisk/sip_general_additional.conf
#touch /etc/asterisk/sip_additional.conf


########### DO NOTHING ###########

#echo 'CREATE DATABASE IF NOT EXISTS `asterisk`;' | mysql
#
## TODO: treat upgrades
#mysql asterisk </usr/pluto/install/asterisk.sql
#
## add asterisk user
#echo "GRANT ALL PRIVILEGES ON asterisk.* to 'asterisk'@'localhost';" | mysql
#echo "FLUSH PRIVILEGES;" | mysql
#
## update res_odbc.conf
#: >/etc/asterisk/res_odbc.conf
#
#echo "[asterisk]
#dsn => MySQL-asterisk
#username => asterisk
#password =>
#pre-connect => yes" >> /etc/asterisk/res_odbc.conf
#
## update extconfig.conf
#: >/etc/asterisk/extconfig.conf
#
#echo "[settings]
#sipusers => odbc,asterisk,sip_buddies
#sippeers => odbc,asterisk,sip_buddies
#iaxusers => odbc,asterisk,iax_buddies
#iaxpeers => odbc,asterisk,iax_buddies
#realtime_ext => odbc,asterisk,extensions_table
#sip.conf => odbc,asterisk,ast_config
#iax.conf => odbc,asterisk,ast_config" >> /etc/asterisk/extconfig.conf
#
## update extensions
#
#: >/etc/asterisk/extensions.conf
#
#echo "[default]
#switch => Realtime/default@realtime_ext
#[trusted]
#switch => Realtime/trusted@realtime_ext
#[outgoing-extern]
#switch => Realtime/outgoing-extern@realtime_ext
#[outgoing-local]
#switch => Realtime/outgoing-local@realtime_ext
#[outgoing-extern-selectline]
#switch => Realtime/outgoing-extern-selectline@realtime_ext
#[outgoing-place-call]
#switch => Realtime/outgoing-place-call@realtime_ext
#[incoming-local]
#switch => Realtime/incoming-local@realtime_ext
#[incoming-place-call]
#switch => Realtime/incoming-place-call@realtime_ext
#[registered-lines]
#switch => Realtime/registered-lines@realtime_ext" >> /etc/asterisk/extensions.conf
#
## update manager.conf
#: >/etc/asterisk/manager.conf
#
#echo "[general]
#enabled = yes
#port = 5038
#bindaddr = 0.0.0.0
#
#[admin]
#secret = adminsecret
#read = system,call,log,verbose,command,agent,user
#write = system,call,log,verbose,command,agent,user" >> /etc/asterisk/manager.conf
#
## update odbcinst.ini
#grep -F "[MySQL]" /etc/odbcinst.ini >/dev/null || echo "[MySQL]
#Description = MySQL
#Driver = /usr/lib/odbc/libmyodbc.so
#FileUsage  = 1" >> /etc/odbcinst.ini
#
## update odbc.ini
#grep -F "[MySQL-asterisk]" /etc/odbc.ini >/dev/null || echo "[MySQL-asterisk]
#Description     = MySQL ODBC Driver Testing
#Driver          = MySQL
#Socket          = /var/run/mysqld/mysqld.sock
#Server          = localhost
#User            = asterisk
#Password        =
#Database        = asterisk
#Option          = 3
#" >> /etc/odbc.ini
#
## restart asterisk
#asterisk -rx "restart gracefully" || true
#
#
