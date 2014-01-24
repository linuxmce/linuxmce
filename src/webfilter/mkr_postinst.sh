#!/bin/bash

#DB

#Create config setup file
Create_Config='
#!/bin/bash

#. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/Utils.sh

#. /usr/pluto/bin/Network_Parameters.sh
#. /usr/pluto/bin/LockUtils.sh

#create the config file


echo $Create_Config >/usr/pluto/bin/webfilter_proxy_conf.sh

Alias='
		Alias /dgblocknotice    /var/www/lmce-admin/operations/webfilter_proxy/webfilter/blocknotice.php
         <Directory /var/www/lmce-admin/operations/webfilter_proxy/webfilter>
        php_admin_flag safe_mode off
        </Directory>
'

exit 0

