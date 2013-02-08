#!/usr/bin/perl

my $pluto_conf = "/etc/pluto.conf";


### TODO: get all variables from config?


our $DCERouter="dcerouter";
our $PK_Device="1";
our $MySqlHost="dcerouter";
our $MySqlPort="3306";
our $MySqlUser="root";
our $MySqlPassword="";
our $MySqlDBName="pluto_main";


open(CONF,$pluto_conf) or die "Could not open pluto configfile \"$pluto_conf\"\n";
while(<CONF>)
{
	chomp;
	# In case we have a blank line in the configuration file, we don't want
	# to choke
	if (length($_) > 0) 
	{
		my ($option, $eq, $value) = split(/ /,$_);

		if ($option eq "DCERouter")
		{
			$DCERouter=$value;
		}
		elsif ($option eq "PK_Device")
		{
			$PK_Device=$value;
		}
		elsif ($option eq "MySqlHost")
		{
			$MySqlHost=$value;
		}
		elsif ($option eq "MySqlPort")
		{
			$MySqlPort=$value;
		}
		elsif ($option eq "MySqlUser")
		{
			$MySqlUser=$value;
		}
			elsif ($option eq "MySqlPassword")
		{
			$MySqlPassword=$value;
		}
		elsif ($option eq "MySqlDBName")
		{
			$MySqlDBName=$value;
		}
	}
}
close(CONF);



### &read_pluto_cred();
### &read_pluto_cred("<database_name>");


sub read_pluto_cred()
{
	my $database_name = shift;
	my $DB_CRED="";


	if (!$database_name)
	{
		$database_name=$MySqlDBName;
	}

	if ($MySqlHost)
	{
		$DB_CRED=$DB_CRED."host=$MySqlHost;"
	}
	if ($MySqlPort)
	{
		$DB_CRED=$DB_CRED."port=$MySqlPort;"
	}
	if ($MySqlUser)
	{
		$DB_CRED=$DB_CRED."user=$MySqlUser;"
	}
	if ($MySqlPassword)
	{
		$DB_CRED=$DB_CRED."password=$MySqlPassword;"
	}
	if ($database_name)
	{
		$DB_CRED=$DB_CRED."database=$database_name;"
	}

	if ($DB_CRED)
	{
		$DB_CRED="DBI:mysql:$DB_CRED";
	}
	else
	{
		$DB_CRED="DBI:mysql";
	}

	$DB_CRED
}



1;

