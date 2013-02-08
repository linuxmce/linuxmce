#!/bin/bash
echo
. /usr/pluto/bin/Config_Ops.sh || :
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :
PROCESS="upgrade"
# Check if we have an existing install, by verifying the DeviceTemplate table exists
mysql $MYSQL_DB_CRED $MySqlDBName -e "Select * From Firewall Limit 0,1"||PROCESS="install"
if [ $PROCESS = "install" ]; then
	/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

	Q="alter table \`Document_Comment\` ,change \`Date\` \`Date\` datetime   NOT NULL "
	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q" || /bin/true
	
	(
		cd /usr/pluto/database/
		/usr/pluto/bin/sqlCVS $PLUTO_DB_CRED -n -D $MySqlDBName -r local import
	)
	
	# In the .26 release a change was inadvertently made disabling the UpdatePackages for MD's.  Temporarily restore that
	Q="update Device_StartupScript set Enabled=1 where FK_StartupScript=34;"
	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q"
	
	# The following open up ports 22 and 80 and 3450 (DCERouter)  on the Firewall.  This is for development mode only.  The release versions will have the firewall closed by default
	set +e
	rows="$(mysql -N $MYSQL_DB_CRED $MySqlDBName -e "select * from Firewall")";
	#if [[ -z "$rows" ]]; then
	#		Q="insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '22',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );"
	#	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q"
	#	Q="insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '80',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );"
	#	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q"
	#	Q="insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '3450',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );"
	#	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q"
	#	Q="insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '3306',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );"
	#	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q"
	#	Q="insert into Firewall ( PK_Firewall, Protocol, SourcePort, SourcePortEnd, DestinationPort, DestinationIP, RuleType, psc_id, psc_batch, psc_user, psc_frozen, psc_mod ) values (  NULL,  'tcp',  '8080',  '0',  '0',  '',  'core_input',  NULL,  NULL,  NULL,  '0',  NULL );"
	#	mysql $MYSQL_DB_CRED $MySqlDBName -e "$Q"
	#fi
	set -e
fi
