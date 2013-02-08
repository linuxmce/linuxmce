#!/bin/bash
#
# USAGE:
#  generateDep.sh <0|1|2|3|4|5|6> <start|stop>
#
# DESCRIPTION: 
#  This scripts generates the dependencies Makefiles starting 
#  from the <RUNLEVEL>-<JOB>.mk.template
#

runlevel="$1"
job="$2"

if [ "$runlevel" == "" -o "$job" == "" ] ;then
    echo "Usage $0 <0|1|2|3|4|5|6> <start|stop>"
    exit 1
fi

if [ ! -f /etc/init.d/fastboot/depend/$runlevel-$job.mk.template ] ;then
    echo "$0: Canot find template /etc/init.d/fastboot/depend/$runlevel-$job.mk.template"
    exit 1
fi

# Generate 'available.srv' that contains the available services
echo -n > /tmp/available.srv

if [[ "$job" == "stop" ]]; then
    rcFiles="/etc/rc$runlevel.d/K*"
else
    rcFiles="/etc/rc$runlevel.d/S*"
fi

for rcFile in  $rcFiles
do
    initFile=$( readlink -f $rcFile )
    [ "$(basename $initFile)" == "Pluto_Start_DCERouter.sh" ] && continue
    [ "$(basename $initFile)" == "Pluto_Start_LocalDevices.sh" ] && continue
    [ "$(basename $initFile)" == "Pluto_Start_X.sh" ] && continue
    [ "$(basename $initFile)" == "Pluto_Config_Device_Changes.sh" ] && continue
    [ "$(basename $initFile)" == "mysql" ] && continue
    [ "$(basename $initFile)" == "ssh" ] && continue
    [ "$(basename $initFile)" == "sysklogd" ] && continue
    
    if [[ -x "$initFile" ]]; then
	echo "$(basename $initFile)" >> /tmp/available.srv
    fi
done

# Generate 'posible.srv' that contains all the service existing in
# the dependencies template
cat /etc/init.d/fastboot/depend/$runlevel-$job.mk.template | \
 sed -e '/^#/d;s/://g;' | \
 tr ' \t' '\n\n'        | \
 sort -u                | \
 sed -e 's/ //g;/^$/d;' > /tmp/posible.srv

# Genertate a sed script used for keeping only the available services in
# the dependencies Makefile by removing the posible ones that are not
# available.
grep -vx -f /tmp/available.srv /tmp/posible.srv | \
 sed -e 's/^\(.*\)/s|\\<\1\\>||g;/g;'             > /tmp/fixup.sed
echo 's|^[ \t]*:.*||g;'                        >> /tmp/fixup.sed
echo '/:[ \t]*$/d;'                            >> /tmp/fixup.sed
echo '/^#/d;'                                  >> /tmp/fixup.sed
echo '/^[ \t]*$/d;'                            >> /tmp/fixup.sed

# Write the cleaned up template file to disk
sed -f /tmp/fixup.sed /etc/init.d/fastboot/depend/$runlevel-$job.mk.template > \
 /etc/init.d/fastboot/depend/$runlevel-$job.mk

# Clean up the tmp files ;)
rm -f /tmp/posible.srv
rm -f /tmp/available.srv
rm -f /tmp/fixup.sed
