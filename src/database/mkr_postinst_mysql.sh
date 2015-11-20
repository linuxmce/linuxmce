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
		# Make sure we have a UTF-8 functioning system
		init_connect='SET NAMES utf8; SET collation_connection = utf8_general_ci;' # Set UTF8 for connection
		character-set-server=utf8
		collation-server=utf8_general_ci
		skip-character-set-client-handshake  # Tells to server to ignore client's charset for connetion
		skip-name-resolve
		skip-external-locking
		innodb-flush-log-at-trx-commit = 2
		bind-address=0.0.0.0
		query_cache_limit=16M
		query_cache_size=128M
		EOF

	Q="GRANT ALL PRIVILEGES ON pluto_main.* to 'root'@'127.0.0.1';"
	mysql $MYSQL_DB_CRED -e "$Q"
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"

	service mysql restart
fi
