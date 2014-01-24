#!/bin/bash

#DB


Alias='
		Alias /dgblocknotice    /var/www/lmce-admin/operations/webfilter_proxy/webfilter/blocknotice.php
         <Directory /var/www/lmce-admin/operations/webfilter_proxy/webfilter>
        php_admin_flag safe_mode off
        </Directory>
'

exit 0

