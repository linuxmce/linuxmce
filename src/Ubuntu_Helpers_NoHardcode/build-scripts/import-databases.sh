#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
set -x

function import_databases () {
	local dbdump_main_sqlcvs=$(mktemp)
	local dbdump_myth_sqlcvs=$(mktemp)
	local dbdump_pluto_media=$(mktemp)
	local dbdump_pluto_security=$(mktemp)
	local dbdump_pluto_telecom=$(mktemp)


	## Import pluto_main, pluto_media, pluto_security, pluto_telecom
	DisplayMessage "Getting a sqldump of main_sqlcvs"
	mysqldump -u "$sqlcvs_user" -h "$sqlcvs_host" main_sqlcvs    > "$dbdump_main_sqlcvs"
	DisplayMessage "Getting a sqldump of myth_sqlcvs"
	mysqldump -u "$sqlcvs_user" -h "$sqlcvs_host" myth_sqlcvs    > "$dbdump_myth_sqlcvs"
	DisplayMessage "Getting a sqldump of pluto_media"
	mysqldump -u "$sqlcvs_user" -h "$sqlcvs_host" pluto_media    > "$dbdump_pluto_media"
	DisplayMessage "Getting a sqldump of pluto_security"
	mysqldump -u "$sqlcvs_user" -h "$sqlcvs_host" pluto_security > "$dbdump_pluto_security"
	DisplayMessage "Getting a sqldump of pluto_telecom"
	mysqldump -u "$sqlcvs_user" -h "$sqlcvs_host" pluto_telecom  > "$dbdump_pluto_telecom"

	# Run search and replace over the db dumps
	DisplayMessage "Running Search'n'Replace on the sqldumps (MakeRelase_PrepFiles)"
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
	$MakeRelease_PrepFiles -p $(dirname $dbdump_main_sqlcvs) -e "$(basename $dbdump_main_sqlcvs),$(basename $dbdump_myth_sqlcvs),$(basename $dbdump_pluto_media),$(basename $dbdump_pluto_security),$(basename $dbdump_pluto_telecom)" -c /etc/lmce-build/${flavor}.conf

	
	# pluto_main_build & pluto_main
	DisplayMessage "Importing pluto_main database"
	echo "DROP DATABASE IF EXISTS pluto_main_build;"| mysql -h localhost -u root
	echo "CREATE DATABASE pluto_main_build;"	| mysql -h localhost -u root
	cat "$dbdump_main_sqlcvs"			| mysql -h localhost -u root "pluto_main_build"
	echo "DROP DATABASE IF EXISTS pluto_main;"	| mysql -h localhost -u root
	echo "CREATE DATABASE pluto_main;"		| mysql -h localhost -u root
	cat "$dbdump_main_sqlcvs"			| mysql -h localhost -u root "pluto_main"

	# main_sqlcvs_ubuntu
	DisplayMessage "Importing main_sqlcvs database"
	echo "DROP DATABASE IF EXISTS main_sqlcvs_${flavor};"	| mysql -h localhost -u root
	echo "CREATE DATABASE main_sqlcvs_${flavor};"	| mysql -h localhost -u root
	cat "$dbdump_main_sqlcvs"			| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	echo 'DELETE FROM `Package_Version`;'		| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	echo 'DELETE FROM `Schema`;'			| mysql -h localhost -u root "main_sqlcvs_${flavor}"
	rm -f "$dbdump_main_sqlcvs"

	# myth_sqlcvs_ubuntu
	DisplayMessage "Importing myth_sqlcvs database"
	echo "DROP DATABASE IF EXISTS myth_sqlcvs_${flavor};"	| mysql -h localhost -u root
	echo "CREATE DATABASE myth_sqlcvs_${flavor};"	| mysql -h localhost -u root
	cat "$dbdump_myth_sqlcvs"			| mysql -h localhost -u root "myth_sqlcvs_${flavor}"
	rm -f "$dbdump_myth_sqlcvs"

	# pluto_media
	DisplayMessage "Importing pluto_media database"
	echo "DROP DATABASE IF EXISTS pluto_media;"	| mysql -h localhost -u root
	echo "CREATE DATABASE pluto_media;"		| mysql -h localhost -u root
	cat "$dbdump_pluto_media"			| mysql -h localhost -u root "pluto_media"
	rm -f "$dbdump_pluto_media"

	# pluto_security
	DisplayMessage "Importing pluto_security database"
	echo "DROP DATABASE IF EXISTS pluto_security;"	| mysql -h localhost -u root
	echo "CREATE DATABASE pluto_security;"		| mysql -h localhost -u root 
	cat "$dbdump_pluto_security" 			| mysql -h localhost -u root "pluto_security"
	rm -f "$dbdump_pluto_security"
	
	# pluto_telecom
	DisplayMessage "Importing pluto_telecom database"
	echo "DROP DATABASE IF EXISTS pluto_telecom;"	| mysql -h localhost -u root
	echo "CREATE DATABASE pluto_telecom;"		| mysql -h localhost -u root
	cat "$dbdump_pluto_telecom"			| mysql -h localhost -u root "pluto_telecom"
	rm -f "$dbdump_pluto_telecom"
}

DisplayMessage "*** STEP: Importing Databases"
trap 'Error "Undefined error in $0"' EXIT

import_databases

trap - EXIT
