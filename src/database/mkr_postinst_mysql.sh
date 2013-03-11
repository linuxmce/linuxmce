#!/bin/bash

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/Config_Ops.sh || :

/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

#enable MySQL networking
MyCnf=/etc/mysql/my.cnf
DefTableType=innodb
if ! BlacklistConfFiles "$MyCnf" ;then
	if [ ! -e /etc/mysql/my.cnf.pbackup ] ;then
		cp "$MyCnf" "$MyCnf".pbackup || :
	fi
	sed -i "s/^skip-networking/#skip-networking/; s/^skip-innodb/#skip-innodb/; s/^default-table-type=.*$/default-table-type=$DefTableType/" "$MyCnf"
	sed -i 's/^bind-address.*$/bind-address=0.0.0.0/; s/\(^log.*=.*$\)/#\1/g' "$MyCnf"
	sed -i 's/^expire_logs_days/#expire_logs_days/g' "$MyCnf"
# removed for precise
#	grep -q '^default-table-type=' "$MyCnf" || echo "default-table-type=$DefTableType" >>"$MyCnf"
	grep -q '^skip-name-resolve' "$MyCnf" || sed -i 's/^\[mysqld\].*$/[mysqld]\nskip-name-resolve/g' "$MyCnf"
	# Make sure we have a UTF-8 functioning system
	echo "[mysqld]
# removed for precise
#	default-table-type=$DefTableType
	init_connect='SET NAMES utf8; SET collation_connection = utf8_general_ci;' # Set UTF8 for connection
# removed for precise
#	default-character-set=utf8
	character-set-server=utf8
	collation-server=utf8_general_ci
	skip-character-set-client-handshake  # Tells to server to ignore client's charset for connetion
	skip-name-resolve
	innodb-flush-log-at-trx-commit = 2
	"> /etc/mysql/conf.d/lmce-my.cnf

	Q="GRANT ALL PRIVILEGES ON pluto_main.* to 'root'@'127.0.0.1';"
	mysql $MYSQL_DB_CRED -e "$Q"
	Q="FLUSH PRIVILEGES;"
	mysql $MYSQL_DB_CRED -e "$Q"

	restart mysql || :
fi
