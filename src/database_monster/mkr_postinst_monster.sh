echo 
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
echo 
echo Setting up monster database.
echo ****This may take a long time****
echo This involves merging all the changes from the
echo central database back into your local copy.
echo Please be patient.......
(cd /usr/pluto/database_monster; /usr/pluto/bin/sqlCVS -S 3 -n -D pluto_main -r dce,designer import) || exit $?

