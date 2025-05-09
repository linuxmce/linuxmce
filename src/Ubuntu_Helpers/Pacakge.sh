#!/bin/bash -x

flavor="pluto_debug"
build_dir="/var/plutobuild"
svn_dir="${build_dir}/svn"
svn_url="http://10.0.0.170/"

sql_master_host="10.0.0.150"
sql_master_db="pluto_main"
sql_master_db_mainsqlcvs="main_sqlcvs_$flavor"
sql_master_db_mythsqlcvs="myth_sqlcvs_$flavor"
sql_master_db_media="pluto_media"
sql_master_db_security="pluto_security"
sql_master_db_telecom="pluto_telecom"
sql_master_user="root"

sql_slave_host="127.0.0.1"
sql_slave_db="pluto_main_build"
sql_slave_db_mainsqlcvs="main_sqlcvs_$flavor"
sql_slave_db_mythsqlcvs="myth_sqlcvs_$flavor"
sql_slave_db_media="pluto_media"
sql_slave_db_security="pluto_security"
sql_slave_db_telecom="pluto_telecom"
sql_slave_user="root"

out_dir="${build_dir}/out"
out_dir="/tmp"
mkr_dir="${build_dir}/MakeRelease"


function Build_Package {
	local Package_ID=$1

	export PATH=/usr/lib/ccache:$PATH:${svn_dir}/trunk/src/bin
	export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/trunk/src/lib"
	local MakeRelease="${mkr_dir}/MakeRelease"

	mkdir -p $out_dir

	#-b
	$MakeRelease -h $sql_slave_host -u $sql_slave_user -O $out_dir -D $sql_slave_db -a -o 15 -r 21 -m 1,1108 -k "$Package_ID" -s "${svn_dir}/trunk" -n /
}


Build_Package $1
