#!/usr/bin/perl

use strict;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

my $DB_PL_HANDLE;
my $DB_TC_HANDLE;
my $DB_STATEMENT;
my $DB_SQL;

$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
$DB_TC_HANDLE = DBI->connect(&read_pluto_cred('pluto_telecom')) or die "Can't connect to database: $DBI::errstr\n";

# Check params
unless(defined($ARGV[0]) && defined($ARGV[1]))
{
    print "USAGE :$0 <line_nr> <line_name> <phone_nr>\n";
    exit(-1);
}

# Get Line ID and Name
my $LINE_NR = $ARGV[0];
my $LINE_NAME = $ARGV[1];

$LINE_NAME =~ s/(\w+)[-]out/$1/;

# Get Phone Number and Delete Incoming Route
if (defined($ARGV[2]))
{
	my $PHONE_NR = $ARGV[2];
	`curl 'http://localhost/admin/config.php?display=did&extdisplay=$PHONE_NR/&action=delIncoming' &>/dev/null`;
}

# Begin Special Line Handling
# Delete inphonex Incoming Route
if ( $LINE_NAME eq "inphonex" )
{
	`curl 'http://localhost/admin/config.php?display=did&extdisplay=/&action=delIncoming' &>/dev/null`;
}
# End Special Line Handling

`curl -o /tmp/curl.log 'http://localhost/admin/config.php?display=routing' &> /dev/null`;

open(PAGE,"/tmp/curl.log") or die "Bad thing happend";
my $OUT_ROUTE = "";

while(<PAGE>)
{
    chomp;
    if($_ =~ /<a\s+[^>]+href[=]\"[^"]+?extdisplay[=]([^"]+)\">\d+\s+$LINE_NAME(|[-]out)<\/a>/)
    {
        $OUT_ROUTE = $1;
    }
}

close(PAGE);

# Delete Outbound Route
`curl 'http://localhost/admin/config.php?display=routing&extdisplay=$OUT_ROUTE&action=delroute' &>/dev/null`;

# Delete Trunk
`curl 'http://localhost/admin/config.php?display=trunks&extdisplay=OUT_$LINE_NR&action=deltrunk' &>/dev/null`;

# Delete LinuxMCE Incoming Line Settings
&delete_from_Line_HouseMode();

# Reload
`curl 'http://localhost/admin/config.php?handler=reload' &>/dev/null`;

sub delete_from_Line_HouseMode()
{
    my $DB_SQL;
    my $DB_STATEMENT;

    $DB_SQL = "DELETE FROM Line_HouseMode WHERE ID=$LINE_NR";
    $DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

    $DB_STATEMENT->finish();
}
