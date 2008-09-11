(cd /usr/pluto/database; /usr/pluto/bin/sqlCVS -n -D pluto_telecom -r telecom import) || exit $?
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

Q="GRANT ALL PRIVILEGES ON pluto_telecom.* TO 'plutotelecom'@'localhost';"
echo "$Q" | /usr/bin/mysql

Q="GRANT ALL PRIVILEGES ON pluto_telecom.* TO 'root'@'127.0.0.1'; FLUSH PRIVILEGES;"
echo "$Q" | /usr/bin/mysql

