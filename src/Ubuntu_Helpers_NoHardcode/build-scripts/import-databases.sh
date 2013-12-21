#!/bin/bash

. /etc/lmce-build/builder.conf
. /usr/local/lmce-build/common/logging.sh

set -e
#set -x


SqlCvsCred=""
if [ "$sqlcvs_host" ] ; then SqlCvsCred="$SqlCvsCred -h$sqlcvs_host"; fi
if [ "$sqlcvs_port" ] ; then SqlCvsCred="$SqlCvsCred -P$sqlcvs_port"; fi
if [ "$sqlcvs_user" ] ; then SqlCvsCred="$SqlCvsCred -u$sqlcvs_user"; fi
if [ "$sqlcvs_pass" ] ; then SqlCvsCred="$SqlCvsCred -p$sqlcvs_pass"; fi

BuildCred=""
if [ "$sql_build_host" ] ; then BuildCred="$BuildCred -h$sql_build_host"; fi
if [ "$sql_build_port" ] ; then BuildCred="$BuildCred -P$sql_build_port"; fi
if [ "$sql_build_user" ] ; then BuildCred="$BuildCred -u$sql_build_user"; fi
if [ "$sql_build_pass" ] ; then BuildCred="$BuildCred -p$sql_build_pass"; fi


function get_dbdump 
{
	local dbdump_name="$1"
	local dbdump_file="$2"


	DisplayMessage "Getting a sqldump of ${dbdump_name}"

#	[ ! -e "${dbdump_file}.prep" ] && #ZaercTempHack
	mysqldump --compress --skip-extended-insert $SqlCvsCred "${dbdump_name}" > "${dbdump_file}.prep"

	cp -f "${dbdump_file}.prep" "${dbdump_file}"

}

function import_dbdump
{
	local dbdump_name="$1"
	local dbdump_file="$2"


	DisplayMessage "Importing ${dbdump_name} database"

	mysql $BuildCred -e "DROP DATABASE IF EXISTS ${dbdump_name}"
	mysql $BuildCred -e "CREATE DATABASE ${dbdump_name}"
	mysql $BuildCred "${dbdump_name}" <"$dbdump_file"

}

function import_databases () {
#	local dbdump_dir=$(mktemp -d)
	local dbdump_dir="${build_dir}/tmp/databases"
	local dbdump_main_sqlcvs="${dbdump_dir}/main_sqlcvs.sql"
	local dbdump_myth_sqlcvs="${dbdump_dir}/myth_sqlcvs.sql"
	local dbdump_pluto_media="${dbdump_dir}/pluto_media.sql"
	local dbdump_pluto_security="${dbdump_dir}/pluto_security.sql"
	local dbdump_pluto_telecom="${dbdump_dir}/pluto_telecom.sql"
	local dbdump_lmce_game="${dbdump_dir}/lmce_game.sql"

	mkdir -p "${dbdump_dir}"


	## Import pluto_main, pluto_media, pluto_security, pluto_telecom
	get_dbdump main_sqlcvs_utf8 "$dbdump_main_sqlcvs"
	get_dbdump myth_sqlcvs "$dbdump_myth_sqlcvs"
	get_dbdump pluto_media "$dbdump_pluto_media"
	get_dbdump pluto_security "$dbdump_pluto_security"
	get_dbdump pluto_telecom "$dbdump_pluto_telecom"
	get_dbdump lmce_game "$dbdump_lmce_game"

#	# Run search and replace over the db dumps
#	DisplayMessage "Running Search'n'Replace on the sqldumps (MakeRelase_PrepFiles)"
#	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"
#	MakeRelease_PrepFiles="${mkr_dir}/MakeRelease_PrepFiles"
#	$MakeRelease_PrepFiles -p $(dirname $dbdump_main_sqlcvs) -e "$(basename $dbdump_main_sqlcvs),$(basename $dbdump_myth_sqlcvs),$(basename $dbdump_pluto_media),$(basename $dbdump_pluto_security),$(basename $dbdump_pluto_telecom)" -c /etc/lmce-build/${flavor}.conf

	# pluto_main_build & pluto_main
	import_dbdump pluto_main_build "$dbdump_main_sqlcvs"
	import_dbdump pluto_main "$dbdump_main_sqlcvs"

	# main_sqlcvs_ubuntu
	import_dbdump "main_sqlcvs_${flavor}" "$dbdump_main_sqlcvs"
	mysql $BuildCred "main_sqlcvs_${flavor}" -e 'DELETE FROM `Package_Version`;'
	mysql $BuildCred "main_sqlcvs_${flavor}" -e 'DELETE FROM `Schema`;'

	# myth_sqlcvs_ubuntu
	import_dbdump "myth_sqlcvs_${flavor}" "$dbdump_myth_sqlcvs"

	# pluto_media
	import_dbdump pluto_media "$dbdump_pluto_media"

	# pluto_security
	import_dbdump pluto_security "$dbdump_pluto_security"

	# pluto_telecom
	import_dbdump pluto_telecom "$dbdump_pluto_telecom"

	# lmce_game
	import_dbdump lmce_game "$dbdump_lmce_game"

#	rm -f "$dbdump_main_sqlcvs" "$dbdump_myth_sqlcvs" "$dbdump_pluto_media" "$dbdump_pluto_security" "$dbdump_pluto_telecom"
#	rm -rf "${dbdump_dir}"

}

DisplayMessage "*** STEP: Importing Databases"
trap 'Error "Undefined error in $0"' EXIT

import_databases

trap - EXIT
