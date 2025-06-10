#!/bin/bash

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh || :

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

#enable MySQL networking
if [[ -e /etc/mysql/mysql.conf.d/mysqld.cnf ]] ; then
	MyCnf=/etc/mysql/mysql.conf.d/mysqld.cnf
else
	MyCnf=/etc/mysql/my.cnf
fi
DefTableType=innodb
if ! BlacklistConfFiles "$MyCnf" ;then
	if [ ! -e "${MyCnf}.pbackup" ] ;then
		cp "$MyCnf" "$MyCnf".pbackup || :
	fi
	sed -i "s/^skip-networking/#skip-networking/; s/^skip-innodb/#skip-innodb/;" "$MyCnf"
	sed -i 's/^bind-address.*$/bind-address=0.0.0.0/; s/\(^log.*=.*$\)/#\1/g' "$MyCnf"
	sed -i 's/^expire_logs_days/#expire_logs_days/g' "$MyCnf"
	grep -q '^skip-name-resolve' "$MyCnf" || sed -i 's/^\[mysqld\].*$/[mysqld]\nskip-name-resolve/g' "$MyCnf"

	cat <<-EOF > /etc/mysql/conf.d/lmce.cnf
		[mysqld]
		# Make sure we have a UTF-8 functioning system
		init_connect='SET NAMES utf8; SET collation_connection = utf8_general_ci;' # Set UTF8 for connection
		character-set-server=utf8
		collation-server=utf8_general_ci
		# We remove the skip-character-set-client-handshake, as it causes issues with MySQL 5.7 and sqlCVS
		# skip-character-set-client-handshake  # Tells to server to ignore client's charset for connetion
		skip-name-resolve
		skip-external-locking
		innodb-flush-log-at-trx-commit = 2
		bind-address=0.0.0.0
		## no longer supported by mysql
		#query_cache_limit=16M
		#query_cache_size=128M
		secure-file-priv = ""
		EOF
		service mysql restart
fi
if [ "x$MySqlUser" == "x" ] ; then
	MySqlUser=root
fi
# Added user create, as mysql auth has changed. -tschak
echo "Creating MySQL user $MySqlUser and asteriskuser"
for NEWUSER in $MySqlUser 'asteriskuser' 'plutosecurity' 'plutotelecom' 'plutomedia' 
do
	# We need both 127.0.0.1 and localhost to work.
	## mysql 8.2+ fails if the user already exists.
	#Q="CREATE USER '$NEWUSER'@'127.0.0.1'; CREATE USER '$NEWUSER'@'localhost';"
	Q="CREATE USER IF NOT EXISTS '$NEWUSER'@'127.0.0.1'; CREATE USER IF NOT EXISTS '$NEWUSER'@'localhost';"
	# If it fails we continue with the grants.
	mysql $MYSQL_DB_CRED -e "$Q" || :
done
			
# Added user create, part 2 -tschak
## mysql 8.2+ doesn't permit PASSWORD()
#Q="SET PASSWORD FOR '$MySqlUser'@'127.0.0.1' = PASSWORD('$MySqlPassword')"
Q="SET PASSWORD FOR '$MySqlUser'@'127.0.0.1' = '$MySqlPassword'"
mysql $MYSQL_DB_CRED -e "$Q"

# errors on noble -phenigma
# not happy about this but it should restore the old functionality for
# xenial / mysql-5.7 - linuxmce issues #2721 #2762 -gavlee
# noble - update user doesn't work for this anymore -phenigma
#Q="update user set authentication_string=password('$MySqlPassword'), plugin='mysql_native_password' where user='$MySqlUser';"
Q="ALTER USER '$MySqlUser' IDENTIFIED WITH mysql_native_password BY '$MySqlPassword';"
mysql $MYSQL_DB_CRED -e "$Q" || :

# the pluto_main database does not exist at this point on a new install.
# add an empty database so the rest of the script and installed packages
# do not error out because the database doesn't exist. -gavlee
Q="CREATE DATABASE IF NOT EXISTS $MySqlDBName;"
mysql $MYSQL_DB_CRED -e "$Q"

# Even if we do not modify the my.cnf file (ie. blacklist it),
# we still want all the grants to hapen.
Q="GRANT ALL PRIVILEGES ON $MySqlDBName.* to '$MySqlUser'@'127.0.0.1';GRANT ALL PRIVILEGES ON $MySqlDBName.* to '$MySqlUser'@'localhost';"
mysql $MYSQL_DB_CRED -e "$Q"

Q="GRANT FILE, SHOW DATABASES ON *.* TO 'asteriskuser'@'127.0.0.1';GRANT FILE, SHOW DATABASES ON *.* TO 'asteriskuser'@'localhost';"
mysql $MYSQL_DB_CRED -e "$Q"

# errors on noble - phenigma
# try to restore asterisk functionality for xenial / mysql-5.7
# as access is denied for asterisk, /etc/asterisk/res_mysql.conf
# contains this info. the hardcoding is not nice but try this for now.
# linuxmce issue #2788 -gavlee
# noble - update user doesn't work for this anymore, already set to caching_sha2_password -phenigma
#Q="update user set authentication_string=password('lmce'), plugin='mysql_native_password' where user='asteriskuser';"
#mysql $MYSQL_DB_CRED -e "$Q"

Q="FLUSH PRIVILEGES;"
mysql $MYSQL_DB_CRED -e "$Q"

service mysql restart
