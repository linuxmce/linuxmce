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
	"lucid")
		Distro_ID="18"
		;;
esac


export PATH=$PATH:${svn_dir}/${svn_branch_name}/src/bin
export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"

set | grep arch
read


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

# Compile the packages
arch=$arch "${mkr_dir}/MakeRelease" -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1 -k "$1" -s "${svn_dir}/${svn_branch_name}" -n / -d
arch=$arch "${mkr_dir}/MakeRelease" -R "$SVNrevision" $PLUTO_BUILD_CRED -O "$out_dir" -D 'pluto_main_build' -o "$Distro_ID" -r 21 -m 1108 -k "$1" -s "${svn_dir}/${svn_branch_name}" -n / -d

