#!/bin/bash

. /etc/lmce-build/builder.conf

set -e
set -x

export PATH=$PATH:${svn_dir}/${svn_branch_name}/src/bin
export LD_LIBRARY_PATH="$mkr_dir:${svn_dir}/${svn_branch_name}/src/lib"

SVNrevision=$(svn info "$svn_dir/$svn_branch_name/src" |grep ^Revision | cut -d" " -f2)

# Compile the packages
"${mkr_dir}/MakeRelease" -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o 16 -r 21 -m 1 -k "$1" -s "${svn_dir}/${svn_branch_name}" -n / -d
"${mkr_dir}/MakeRelease" -R "$SVNrevision" -h 'localhost' -u 'root' -O "$out_dir" -D 'pluto_main_build' -o 16 -r 21 -m 1108 -k "$1" -s "${svn_dir}/${svn_branch_name}" -n / -d

