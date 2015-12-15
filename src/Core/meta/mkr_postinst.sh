#!/bin/bash

. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/install/install-core.sh

###. /usr/pluto/install/install-core.sh ; Config_MySQL_Server
# grant debian-sys-main privs in mysql
#Run mysql statement
echo "UPDATE user SET Create_view_priv = 'Y', Show_view_priv = 'Y', \
	Create_routine_priv = 'Y', Alter_routine_priv = 'Y', \
	Create_user_priv = 'Y' WHERE User = 'debian-sys-maint'; \
	FLUSH PRIVILEGES; \
	" | mysql --defaults-extra-file=/etc/mysql/debian.cnf mysql

# Raise max char limit on php.ini
if ! BlacklistConfFiles '/etc/php5/apache2/php.ini' ;then
	if ! grep -q 'max_input_vars' /etc/php5/apache2/php.ini; then
		echo "max_input_vars = 15000" >> /etc/php5/apache2/php.ini
	fi
fi

if ! BlacklistConfFiles '/etc/hostname' ;then
	if [ "$(/bin/hostname)" != "dcerouter" ] ; then
		echo dcerouter > /etc/hostname
	fi
fi

###. /usr/pluto/install/install-core.sh ; Setup_Pluto_Conf	# lmce-install-scripts - install-core.sh
if ! BlacklistConfFiles '/etc/pluto.conf' ;then
	StatsMessage "Updating /etc/pluto.conf (if required)"
	touch /etc/pluto.conf &>/dev/null
	chmod 777 /etc/pluto.conf &>/dev/null
	. /usr/pluto/bin/Config_Ops.sh

	[ -n "$MySqlHost" ] || ConfSet "MySqlHost" "localhost"
	[ -n "$MySqlUser" ] || ConfSet "MySqlUser" "root"
	[ -n "$MySqlPassword" ] || ConfSet "MySqlPassword" ""
	[ -n "$MySqlDBName" ] || ConfSet "MySqlDBName" "pluto_main"
	[ -n "$DCERouter" ] || ConfSet "DCERouter" "localhost"
	[ -n "$MySqlPort" ] || ConfSet "MySqlPort" "3306"
	[ -n "$DCERouterPort" ] || ConfSet "DCERouterPort" "3450"
	[ -n "$PK_Device" ] || ConfSet "PK_Device" "1"
	[ -n "$Activation_Code" ] || ConfSet "Activation_Code" "1111"
	[ -n "$PK_Installation" ] || ConfSet "PK_Installation" "1"
	[ -n "$PK_Users" ] || ConfSet "PK_Users" "1"
	[ -n "$PK_Distro" ] || ConfSet "PK_Distro" "$TARGET_DISTRO_ID"
	[ -n "$UseVideoWizard" ] || ConfSet "UseVideoWizard" "1"
	[ -n "$LogLevels" ] || ConfSet "LogLevels" "1,5,7,8"
	ConfSet "AutostartCore" "1"
	[ -n "$AutostartMedia" ] || ConfSet "AutostartMedia" "0"
	[ -n "$LTS_HES" ] || ConfSet "LTS_HES" "$TARGET_LTS_HES"
fi

# Do we have an installation ID already?
Q="SELECT COUNT(*) FROM Installation"
R=$(RunSQL "$Q")
if [ "$R" -lt "1" ] ; then
	## Update some info in the database
	Q="INSERT INTO Installation(Description, ActivationCode) VALUES('LinuxMCE', '1111')"
	RunSQL "$Q"
fi

exit 0
