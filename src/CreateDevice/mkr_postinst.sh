#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

# TODO: remove this?  This is a relic of database based installation
# where the database existed prior to package installation.
/usr/pluto/bin/CreateDevice -h dcerouter -r 2>/dev/null >/dev/null || :

