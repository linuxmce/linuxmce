#!/bin/bash

. /usr/pluto/bin/Utils.sh

###. /usr/pluto/install/install-core.sh ; Config_MySQL_Server
# grant debian-sys-main privs in mysql
#Run mysql statement
echo "UPDATE user SET Create_view_priv = 'Y', Show_view_priv = 'Y', \
	Create_routine_priv = 'Y', Alter_routine_priv = 'Y', \
	Create_user_priv = 'Y' WHERE User = 'debian-sys-maint'; \
	FLUSH PRIVILEGES; \
	" | mysql --defaults-extra-file=/etc/mysql/debian.cnf mysql

###. /usr/pluto/install/install-core.sh ; Setup_Hostname
if ! BlacklistConfFiles '/etc/hostname' ;then
	# set the hostname to dcerouter
	echo dcerouter > /etc/hostname
fi

# Raise max char limit on php.ini
if ! BlacklistConfFiles '/etc/php5/apache2/php.ini' ;then
	if ! grep -q 'max_input_vars' /etc/php5/apache2/php.ini; then
		echo "max_input_vars = 15000" >> /etc/php5/apache2/php.ini
	fi
fi

exit 0
