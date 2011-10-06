#!/usr/bin/perl

# by foxi352 - september 2011:
#
# THIS HAS BEEN REPLACED BY A NEW SHELL SCRIPT.
# THIS FILE STILL EXISTS WAITING FOR EVERY CALL TO IT TO BE REPLACED
#
# TODO: REPLACE EVERY CALL TO THIS FILE BY db_create_telecom_defaults.sh AND DELETE THIS PERL SCRIPT
if (-e '/usr/pluto/bin/db_create_dialplan.sh') {
	`/usr/pluto/bin/db_create_dialplan.sh`;
	exit 0;
}

use strict;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

`/usr/pluto/bin/create_telecom_defaults.pl`;

my $EXT_FILE = "/etc/asterisk/extensions_pluto_dial.conf";
my $EXT_BUFFER = "[ext-local-custom]\ninclude => from-pluto-custom\n\n[custom-linuxmce]\ninclude => from-pluto-custom\n\n[from-pluto-custom]\n";

my %USERS = ();
my %USERS_MB = ();
my %MAILS = ();
my %NAMES = ();
my %PHONES = ();

my $TIMEOUT = 15;

my $DB_PL_HANDLE;
my $DB_TC_HANDLE;
my $DB_STATEMENT;
my $DB_SQL;
my $DB_ROW;

$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
$DB_TC_HANDLE = DBI->connect(&read_pluto_cred('pluto_telecom')) or die "Can't connect to database: $DBI::errstr\n";
&get_all_users_extensions();
&get_all_phones_extensions();
&get_timeout();

print "TIMEOUT:${TIMEOUT}\n";

$EXT_BUFFER .= "\n;Lines\n";
$EXT_BUFFER .= "exten => 100,1,VoiceMail(100)\n";
$EXT_BUFFER .= "exten => 100,n,Hangup\n";

$DB_SQL = "select ID,EK_HouseMode,Routing from Line_HouseMode order by ID,EK_HouseMode";
$DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
my $tmp="";
while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{
    my $line = "10".$1 if($DB_ROW->{'ID'} =~ /(\d)$/);
    my $hm = $DB_ROW->{'EK_HouseMode'};

    if($tmp ne $line)
    {
	$EXT_BUFFER .= "exten => $line,1,AGI(linuxmce-phonebook-lookup.agi)\n";
        $EXT_BUFFER .= "exten => $line,n,AGI(pluto-gethousemode.agi)\n";
        $EXT_BUFFER .= "exten => $line,n,Goto($line-hm\${HOUSEMODE},1)\n";
        $EXT_BUFFER .= "exten => $line,n,Hangup\n";
    }

    my $action = "NoOp(\"Do nothing\")";

    if($DB_ROW->{'Routing'} =~ /^ring,((\d+[,])*(\d+))$/)
    {
        my @arr=split(/,/,$1);
        $action = "Dial(";

        foreach my $i (@arr)
        {
            $action .= "Local/".$PHONES{$i}."\@trusted&";
        }

        $action =~ s/[&]$//;
        $action .= ",$TIMEOUT)";
    }

    if($DB_ROW->{'Routing'} =~ /^user,(\d+)$/)
    {
        $action = "Dial(Local/".$USERS{$1}."\@trusted,$TIMEOUT)";
    }

    if($DB_ROW->{'Routing'} =~ /^transfer,(\d+)$/)
    {
        $action = "Dial(Local/".$1."\@trusted,$TIMEOUT)";
    }

    if($DB_ROW->{'Routing'} =~ /^voicemail,(\d+)$/)
    {
        $action = "Macro(vm,".$USERS{$1}.",DIRECTDIAL)";
    }

    if($DB_ROW->{'Routing'} =~ /^prompt/)
    {
        $action = "Goto(voice-menu-pluto-custom,s,1)";
    }

    $EXT_BUFFER .= "exten => $line-hm$hm,1,$action\n";
    $EXT_BUFFER .= "exten => $line-hm$hm,n,Goto($line-hm$hm-\${DIALSTATUS},1)\n";
    $EXT_BUFFER .= "exten => $line-hm$hm,n,Hangup\n";
    $EXT_BUFFER .= "exten => $line-hm$hm-BUSY,1,Goto(100,1)\n";
    $EXT_BUFFER .= "exten => $line-hm$hm-NOANSWER,1,Goto(voice-menu-pluto-custom,s,1)\n";
    $EXT_BUFFER .= "exten => $line-hm$hm-CONGESTION,1,Goto(100,1)\n";
    $EXT_BUFFER .= "exten => $line-hm$hm-CHANUNAVAIL,1,Goto(100,1)\n";
    $tmp = $line;
}

$EXT_BUFFER .= "\n;Users\n";

$DB_SQL = "select EK_Users,EK_UserMode,IsPriorityCaller,StepOrder,Routing from UserRouting order by EK_Users,IsPriorityCaller,EK_UserMode,StepOrder";
$DB_STATEMENT = $DB_TC_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";

$tmp = "";

while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{{
    next unless(defined $USERS{$DB_ROW->{'EK_Users'}});
    my $user = $USERS{$DB_ROW->{'EK_Users'}};
    my $um = $DB_ROW->{'EK_UserMode'};
    my $try = $DB_ROW->{'StepOrder'};
    my $pri = $DB_ROW->{'IsPriorityCaller'};

    unless($tmp =~ /^$user[-]/)
    {
        $EXT_BUFFER .= "exten => $user,1,AGI(pluto-getusermode.agi)\n";
        $EXT_BUFFER .= "exten => $user,n,Goto($user-um\${USERMODE}-pri\${PRIORITYCALLER},1)\n";
        $EXT_BUFFER .= "exten => $user,n,Hangup\n";
    }

    my $action = "NoOp(\"Do nothing\")";

    if($DB_ROW->{'Routing'} =~ /^ring,((\d+[,])*(\d+))$/)
    {
        my @arr=split(/,/,$1);
        $action = "Dial(";

        foreach my $i (@arr)
        {
            $action .= "Local/".$PHONES{$i}."\@trusted&";
        }

        $action =~ s/[&]$//;
        $action .= ",$TIMEOUT)";
    }

    if($DB_ROW->{'Routing'} =~ /^ring,$/)
    {
        $action = "Dial(";

        foreach my $i (sort(values(%PHONES)))
        {
            $action .= "Local/".$i."\@trusted&";
        }

        $action =~ s/[&]$//;
        $action .= ",$TIMEOUT)";
    }

    if($DB_ROW->{'Routing'} =~ /^user,(\d+)$/)
    {
        $action = "Dial(Local/".$USERS{$1}."\@trusted,$TIMEOUT)";
    }

    if($DB_ROW->{'Routing'} =~ /^transfer,(\d+)$/)
    {
        $action = "Dial(Local/".$1."\@trusted,$TIMEOUT)";
    }

    if($DB_ROW->{'Routing'} =~ /^voicemail,(\d+)$/)
    {
		if(defined $USERS{$1})
		{
       		$action = "Macro(vm,".$USERS{$1}.",DIRECTDIAL)";
		}
		else
		{
			$action = "Macro(vm,".$user.",DIRECTDIAL)";
		}
    }

    if($tmp ne $user."-".$pri."-".$um)
    {
        $EXT_BUFFER .= "exten => $user-um$um-pri$pri,1,Goto($user-um$um-pri$pri-try$try,1)\n";
    }

    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try,1,$action\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try,n,Goto($user-um$um-pri$pri-try$try-\${DIALSTATUS},1)\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try,n,Hangup\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-BUSY,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-NOANSWER,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-CONGESTION,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try$try-CHANUNAVAIL,1,Goto($user-um$um-pri$pri-try".($try+1).",1)\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try".($try+1).",1,Macro(vm,$user,DIRECTDIAL)\n";
    $EXT_BUFFER .= "exten => $user-um$um-pri$pri-try".($try+1).",n,Hangup\n";
    $tmp = $user."-".$pri."-".$um;
}}

$EXT_BUFFER .= "\n;Conference rooms\n";
$EXT_BUFFER .= "exten => _000.,1,Meetme(\${EXTEN}|q)\n";
$EXT_BUFFER .= "exten => _000.,n,Hangup\n";

$EXT_BUFFER .= "\n\n[voice-menu-pluto-custom]\n\n";
$EXT_BUFFER .= "exten => s,1,Answer\n";
$EXT_BUFFER .= "exten => s,n,Wait(1)\n";
$EXT_BUFFER .= "exten => s,n,AGI(pluto-callersforme.agi)\n";
$EXT_BUFFER .= "exten => s,n,Background(pluto/pluto-default-voicemenu)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(digit)=10)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(response)=20)\n";
$EXT_BUFFER .= "exten => t,1,Goto(s,1)\n";

$tmp="";

foreach my $phone (sort (values(%PHONES)))
{
    $tmp .= "Local/$phone\@trusted&";
}

$tmp =~ s/[&]$//;

$EXT_BUFFER .= "exten => 0,1,Dial($tmp,$TIMEOUT)\n";
$EXT_BUFFER .= "exten => 0,n,Goto(s,1)\n";

foreach my $user (sort (values(%USERS)))
{
    $EXT_BUFFER .= "exten => $1,1,GoTo(from-pluto-custom,$user,1)\n" if ($user =~ /(\d)$/);
    $EXT_BUFFER .= "exten => $1,n,Goto(s,1)\n" if ($user =~ /(\d)$/);
}

$EXT_BUFFER .= "exten => #,1,VoiceMail(100)\n";
$EXT_BUFFER .= "exten => #,n,Hangup\n";
$EXT_BUFFER .= "exten => _XXX,1,Dial(Local/\${EXTEN}\@trusted)\n";
$EXT_BUFFER .= "exten => _XXX,n,Hangup\n";
$EXT_BUFFER .= "exten => i,1,Background(pluto/invalid-entry)\n";
$EXT_BUFFER .= "exten => i,n,Goto(s,1)\n";

open(FILE,"> $EXT_FILE") or die "Could not open '$EXT_FILE'";
print FILE $EXT_BUFFER;
close(FILE);

# Create voiceboxes
my $VM_FILE = "/etc/asterisk/voicemail.conf";
my $VM_BUFFER = "";

open(FILE,"< $VM_FILE") or die "Could not open '$VM_FILE'";

while(<FILE>)
{
    chomp;
    my $line = $_;
    if($line =~ /^\[default\]$/)
    {
        $VM_BUFFER .= $line."\n";
        $line = <FILE>;
        chomp $line;
        my $founduser= ",";
        my $user;
        my $key;
        while($line =~ /[^\[]/)
        {
            foreach $key (keys (%USERS_MB))
            {
                $user = $USERS_MB{$key};
                if($line =~ /^$user\s[=][>]/)
                {
                    $VM_BUFFER .= $line."\n";
                    $founduser .= $user.",";
                }
            }
            if($line =~ /^100\s[=][>]/)
            {
                $VM_BUFFER .= $line."\n";
                $founduser .= "100,";
            }

            $line = <FILE>;
            chomp $line;
        }
        foreach $key (keys (%USERS_MB))
        {
            $user = $USERS_MB{$key};
            unless($founduser =~ /,$user,/)
            {
                $VM_BUFFER .= "$user => $user,".$NAMES{$key}.",".$MAILS{$key}.",,attach=yes|saycid=no|envelope=yes|delete=no|nextaftercmd=no|operator=no\n";
            }
        }
        unless($founduser =~ /,100,/)
        {
            $VM_BUFFER .= "100 => 100,Default,,,attach=no|saycid=no|envelope=no|delete=no|nextaftercmd=no|operator=no\n";
        }

        $VM_BUFFER .= $line."\n";
    }
    else
    {
        $VM_BUFFER .= $line."\n";
    }
}

close(FILE);

open(FILE,"> $VM_FILE") or die "Could not open '$VM_FILE'";
print FILE $VM_BUFFER;
close(FILE);

sub get_all_users_extensions()
{
    $DB_SQL = "select PK_Users,UserName,Extension,ForwardEmail,HasMailbox from Users where `Extension` like '30%'";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $USERS{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'Extension'};
	if( $DB_ROW->{'HasMailbox'} == 1 )
	{
             $USERS_MB{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'Extension'};
	}
        $MAILS{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'ForwardEmail'};
        $NAMES{$DB_ROW->{'PK_Users'}} = $DB_ROW->{'UserName'};
    }
    $DB_STATEMENT->finish();
}

sub get_all_phones_extensions()
{
    $DB_SQL = "select FK_Device, IK_DeviceData from Device_DeviceData where FK_DeviceData=31";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    while($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
        $PHONES{$DB_ROW->{'FK_Device'}} = $DB_ROW->{'IK_DeviceData'};
    }
    $DB_STATEMENT->finish();
}

sub get_timeout()
{
    # Get timeout 
    $DB_SQL = "SELECT IK_DeviceData,FK_DeviceData FROM Device_DeviceData JOIN Device ON FK_Device = PK_Device WHERE FK_DeviceTemplate =34 and FK_DeviceData=247;";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    if($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
    {
	    
        if($DB_ROW->{'IK_DeviceData'}=="")
	{
	    $TIMEOUT=0;
	}
	else
	{
	    $TIMEOUT=$DB_ROW->{'IK_DeviceData'};        
	}
    }
    $DB_STATEMENT->finish();
}									

# Make sure all files are correctly owned by the asterisk user.
`chown asterisk:asterisk /etc/asterisk/*`;

# restart gracefully works in -1.4
`asterisk -rx "restart gracefully"`;

# core restart gracefully works in 1.6+
`asterisk -rx "core restart gracefully"`;
