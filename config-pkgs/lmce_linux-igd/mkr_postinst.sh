#!/bin/bash
# Configure linux-igd to properly integrate with LMCE.
. /usr/pluto/bin/Network_Parameters.sh
sed -i "s/^INTIFACE=\"*\(.*\)\"*/INTIFACE=${IntIf}/" /etc/default/linux-igd
sed -i "s/^EXTIFACE=\"*\(.*\)\"*/EXTIFACE=${ExtIf}/" /etc/default/linux-igd
/etc/init.d/linux-igd restart
