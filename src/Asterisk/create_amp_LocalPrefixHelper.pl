#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

sub get_local_prefixes()
{
    my $DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
    my $DB_STATEMENT;
    my $DB_SQL;
    my $DB_ROW;

    my $DECLARED_PREFIX = $_[0];

    my $LOCAL_PREFIX1 = "";
    my $LOCAL_PREFIX2 = "";

    $LOCAL_PREFIX1 = "112\n411\n911\n9|.\n";
    $LOCAL_PREFIX2 = $LOCAL_PREFIX1;
    $DB_SQL = "SELECT IK_DeviceData,FK_DeviceData FROM Device_DeviceData JOIN Device ON FK_Device=PK_Device WHERE FK_DeviceTemplate=34 AND (FK_DeviceData=141 OR FK_DeviceData=142 OR FK_DeviceData=143) ORDER BY FK_DeviceData;";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

    return unless($DB_ROW = $DB_STATEMENT->fetchrow_hashref());
    return unless($DB_ROW->{'FK_DeviceData'} == 141);
    if($DB_ROW->{'IK_DeviceData'} > 0)
    {
        my $prefix = $DB_ROW->{'IK_DeviceData'};
        return unless($DB_ROW = $DB_STATEMENT->fetchrow_hashref());
        return unless($DB_ROW->{'FK_DeviceData'} == 142);
        my $digit = $DB_ROW->{'IK_DeviceData'};
        return unless($DB_ROW = $DB_STATEMENT->fetchrow_hashref());
        return unless($DB_ROW->{'FK_DeviceData'} == 143);
        my $length = $DB_ROW->{'IK_DeviceData'};
        my $short = "";
        my $long = "";
        for( my $i=0;$i<$length;$i++)
        {
            $short .= "X";
            $long .= "X";
        }
        for( my $i=0;$i<length($prefix);$i++)
        {
            $long .= "X";
        }
        $LOCAL_PREFIX1 =~ s/\n[^\n]+\n$/\n/s;
        $LOCAL_PREFIX2 =~ s/\n[^\n]+\n$/\n/s;
        $LOCAL_PREFIX1 .= ($digit<0?"":$digit.$prefix."+").$short."\n";
        $LOCAL_PREFIX1 .= ($digit<0?"":$digit."+").$long."\n";
        $LOCAL_PREFIX1 .= $DECLARED_PREFIX."|.\n";
	$LOCAL_PREFIX2 .= $DECLARED_PREFIX."|".$short."\n";
	$LOCAL_PREFIX2 .= $DECLARED_PREFIX."|".$long."\n";
        $LOCAL_PREFIX2 .= $short."\n";
        $LOCAL_PREFIX2 .= $long."\n";
        $LOCAL_PREFIX2 .= "9|112\n9|411\n9|911\n";
        $LOCAL_PREFIX2 .= "9|".($digit<0?"":$digit).$long."\n";
	$LOCAL_PREFIX2 .= "9|0.\n9|*.\n";		
    }
    return ($LOCAL_PREFIX1, $LOCAL_PREFIX2);
}
