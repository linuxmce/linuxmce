#!/bin/bash

. /etc/lmce-build/builder.conf

set -e
set -x


case "${build_name}" in
	"gutsy")
		Distro_ID="15"
		;;
	"hardy")
		Distro_ID="16"
		;;
	"intrepid")
		Distro_ID="17"
		;;
esac


export PATH=$PATH:${svn_dir}/${svn_branch_name}/src/bin
export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"

PLUTO_BUILD_CRED=""
if [ "$sql_build_host" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -h $sql_build_host"; fi
if [ "$sql_build_port" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -P $sql_build_port"; fi
if [ "$sql_build_user" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -u $sql_build_user"; fi
if [ "$sql_build_pass" ] ; then PLUTO_BUILD_CRED="$PLUTO_BUILD_CRED -p $sql_build_pass"; fi
export PLUTO_BUILD_CRED

MYSQL_BUILD_CRED=""
if [ "$sql_build_host" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -h$sql_build_host"; fi
if [ "$sql_build_port" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -P$sql_build_port"; fi
if [ "$sql_build_user" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -u$sql_build_user"; fi
if [ "$sql_build_pass" ] ; then MYSQL_BUILD_CRED="$MYSQL_BUILD_CRED -p$sql_build_pass"; fi
export MYSQL_BUILD_CRED


SVNrevision=$(svn info "$svn_dir/$svn_branch_name/src" |grep ^Revision | cut -d" " -f2)

# Set version of packages to todays date, plus 00:19 as time
Q="Update Version Set VersionName= concat('2.0.0.44.',substr(now()+0,3,6),'$SVNrevision') Where PK_Version = 1;"
mysql $PLUTO_BUILD_CRED -D 'pluto_main_build' -e "$Q"

# Compile the packages
"${mkr_dir}/MakeRelease" -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1 -k "$1" -s "${svn_dir}/${svn_branch_name}" -n / -d
"${mkr_dir}/MakeRelease" -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1108 -k "$1" -s "${svn_dir}/${svn_branch_name}" -n / -d

