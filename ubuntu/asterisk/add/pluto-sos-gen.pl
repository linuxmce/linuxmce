#!/usr/bin/perl -w

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

my $EXT_FILE = "/etc/asterisk/extensions_custom.conf";
my $EXT_BUFFER = "";
my $TMP_BUFFER = "";

my $db_handle;
my $sql;
my $statement;
my $row;
my @data;

# Connect to pluto_main database
$db_handle = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
$sql = "select PK_Device from Device where FK_DeviceTemplate=33;";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO SECURITY PLUGIN\n";
    exit(1);
}

$statement->finish();

$sql = "select IK_DeviceData from Device_DeviceData where FK_Device=\'".$row->{PK_Device}."\' and FK_DeviceData='96';";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
unless($row = $statement->fetchrow_hashref())
{
    print STDERR "NO DEVICEDATA FOR SECURITY PLUGIN\n";
    exit(1);
}

$statement->finish();

@data = split(/[,]/,$row->{IK_DeviceData});

print "Generating speech for People to Call\n";
my $list = "";
for(my $i=0;defined($data[$i]);$i+=2)
{
	my $j=int(($i+2)/2);
	&generate_voice("To call the $data[$i] press $j","/tmp/pluto-security-sos-option$j.gsm");
	$list .= "/tmp/pluto-security-sos-option$j.gsm ";

	$TMP_BUFFER .= "exten => $j,1,Dial(Local/9$data[$i -1]\@trusted\n";
	$TMP_BUFFER .= "exten => $j,n,Hangup\n\n";
}

`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/pluto-security-sos-options.gsm`;

# Begin writing extensions_custom.conf
$EXT_BUFFER .= "; This file contains example extensions_custom.conf entries.\n";
$EXT_BUFFER .= "; extensions_custom.conf should be used to include customizations\n";
$EXT_BUFFER .= "; to AMP's Asterisk dialplan.\n\n";
$EXT_BUFFER .= "; All custom context should contain the string 'custom' in it's name\n\n";
$EXT_BUFFER .= "; Extensions in AMP have access to the 'from-internal' context.\n";
$EXT_BUFFER .= "; The context 'from-internal-custom' is included in 'from-internal' by default\n\n";

$EXT_BUFFER .= "[trusted]\n";
$EXT_BUFFER .= "include => from-internal\n";

$EXT_BUFFER .= "[from-internal-custom]\n";
$EXT_BUFFER .= "exten => 997,1,Answer\n";
$EXT_BUFFER .= "exten => 997,n,Wait(1)\n";
$EXT_BUFFER .= "exten => 997,n,Set(TIMEOUT(absolute)=150)\n";
$EXT_BUFFER .= "exten => 997,n,MeetMe(997,q)\n";
$EXT_BUFFER .= "exten => 997,n,Hangup\n\n";

$EXT_BUFFER .= "exten => 998,1,Goto(pluto-console-custom,s,1)\n";
$EXT_BUFFER .= "exten => 998,n,Hangup\n\n";

$EXT_BUFFER .= "exten => 999,1,Goto(linuxmce-validate-user,s,1)\n";
$EXT_BUFFER .= "exten => 999,n,Hangup\n\n";

$EXT_BUFFER .= "[linuxmce-validate-user]\n";
$EXT_BUFFER .= "exten => s,1,Answer\n";
$EXT_BUFFER .= "exten => s,n,Wait(1)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(digit)=14)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(response)=10)\n";
$EXT_BUFFER .= "exten => s,n,Background(pluto/userselect)\n\n";

# Generating Speech for User Selection
print "Generating speech for User Selection\n";
$list = "";
&generate_voice("Please tell me who you are.","/tmp/user.gsm");
$list .= "/tmp/user.gsm ";

$sql = "select if((Nickname=\"\" OR Nickname IS NULL),UserName,Nickname) AS Name, Extension from Users";
$statement = $db_handle->prepare($sql) or die "Couldn't prepare query '$sql': $DBI::errstr\n";
$statement->execute() or die "Couldn't execute query '$sql': $DBI::errstr\n";
while(my $DB_ROW = $statement->fetchrow_hashref())
{
    my $i=$1 if($DB_ROW->{'Extension'} =~ /(\d)$/);
    print "Generating speech for ".$DB_ROW->{'Name'}."\n";
    &generate_voice("For ".$DB_ROW->{'Name'}.", dial ".$i.".","/tmp/user$i.gsm");
    $list .= "/tmp/user$i.gsm ";

    $EXT_BUFFER .= "exten => $i,1,SetGlobalVar(USER=".$DB_ROW->{'Name'}.")\n";
    $EXT_BUFFER .= "exten => $i,n,NoOp(\${USER})\n";
    $EXT_BUFFER .= "exten => $i,n,Goto(linuxmce-userpin,s,1)\n";
    $EXT_BUFFER .= "exten => $i,n,Hangup\n\n";
}
$statement->finish();

`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/userselect.gsm`;

# Clean up
unlink "/tmp/user.gsm";

# Generating speech for Hanging Up
print "Generating speech for Hanging Up\n";
$list = "";
&generate_voice("Goodbye.","/tmp/linuxmce-goodbye.gsm");
$list .= "/tmp/linuxmce-goodbye.gsm";

`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/linuxmce-goodbye.gsm`;

# Clean up
unlink "/tmp/linuxmce-goodbye.gsm";

$EXT_BUFFER .= "exten => *,1,Playback(pluto/linuxmce-goodbye)\n";
$EXT_BUFFER .= "exten => *,n,Hangup\n\n";

$EXT_BUFFER .= "exten => #,1,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => t,1,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => i,1,Playback(pluto/invalid-entry)\n";
$EXT_BUFFER .= "exten => i,n,Goto(s,3)\n\n";

$EXT_BUFFER .= "[linuxmce-userpin]\n";
$EXT_BUFFER .= "exten => s,1,Answer\n";
$EXT_BUFFER .= "exten => s,n,Wait(1)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(digit)=3)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(response)=4)\n";

print "Generating speech for Pin Entry\n";
$list = "";
&generate_voice("Please enter your pin.","/tmp/enterpin.gsm");
$list .= "/tmp/enterpin.gsm";
`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/enterpin.gsm`;

# Clean up
unlink "/tmp/enterpin.gsm";

$EXT_BUFFER .= "exten => s,n,Background(pluto/enterpin)\n\n";

$EXT_BUFFER .= "exten => _.,1,SetGlobalVar(PIN=\${EXTEN})\n";
$EXT_BUFFER .= "exten => _.,n,NoOp(Username: \${USER} PIN: \${PIN})\n";
$EXT_BUFFER .= "exten => _.,n,AGI(linuxmce-validate-user-pin.agi,\${USER},\${PIN})\n\n";

$EXT_BUFFER .= "exten => *,1,Goto(linuxmce-validate-user,s,1)\n";
$EXT_BUFFER .= "exten => *,n,Hangup\n\n";

$EXT_BUFFER .= "exten => #,1,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => t,1,Goto(s,3)\n\n";

# Generating speech for Invalid Pin
print "Generating speech for Invalid Pin\n";
$list = "";
&generate_voice("Your pin was not valid.  Goodbye.","/tmp/pin-invalid.gsm");
$list .= "/tmp/pin-invalid.gsm ";
`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/pin-invalid.gsm`;

# Clean up
unlink "/tmp/pin-invalid.gsm";

$EXT_BUFFER .= "exten => i,1,Playback(pluto/pin-invalid)\n";
$EXT_BUFFER .= "exten => i,n,Hangup\n\n";

$EXT_BUFFER .= "exten => h,1,Hangup\n\n";

$EXT_BUFFER .= "[pluto-console-custom]\n";
$EXT_BUFFER .= "exten => s,1,System(/usr/pluto/bin/MessageSend localhost -targetType category 0 8 1 192)\n";
$EXT_BUFFER .= "exten => s,n,System(/usr/pluto/bin/pluto-sos-speak.pl)\n";
$EXT_BUFFER .= "exten => s,n,Dial(Local/997\@trusted/n)\n";
$EXT_BUFFER .= "exten => s,n,Hangup\n\n";

$EXT_BUFFER .= "exten => h,1,System(/usr/pluto/bin/MessageSend localhost -targetType category 0 8 1 193)\n";
$EXT_BUFFER .= "exten => h,n,System(killall -12 simplephone)\n\n";

$EXT_BUFFER .= "[pluto-security-custom]\n";
$EXT_BUFFER .= "exten => s,1,Answer\n";
$EXT_BUFFER .= "exten => s,n,Wait(1)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(digit)=14)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(response)=5)\n";

# Generating speech for SOS Menu Options
print "Generating speech for SOS Menu Options\n";
$list = "";
&generate_voice("Linux M C E Security Menu","/tmp/linuxmce-security.gsm");
$list .= "/tmp/linuxmce-security.gsm ";
&generate_voice("To reset the alarm dial 1","/tmp/linuxmce-security1.gsm");
$list .= "/tmp/linuxmce-security1.gsm ";
&generate_voice("To ignore the alarm dial 2","/tmp/linuxmce-security2.gsm");
$list .= "/tmp/linuxmce-security2.gsm ";
&generate_voice("To call emergency contacts dial 3","/tmp/linuxmce-security3.gsm");
$list .= "/tmp/linuxmce-security3.gsm ";
&generate_voice("To notify your neighbors dial 4","/tmp/linuxmce-security4.gsm");
$list .= "/tmp/linuxmce-security4.gsm ";
&generate_voice("To speak to the person in your house dial 5","/tmp/linuxmce-security5.gsm");
$list .= "/tmp/linuxmce-security5.gsm";

`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/linuxmce-security-options.gsm`;

# Clean up
unlink "/tmp/linuxmce-security.gsm";
unlink "/tmp/linuxmce-security1.gsm";
unlink "/tmp/linuxmce-security2.gsm";
unlink "/tmp/linuxmce-security3.gsm";
unlink "/tmp/linuxmce-security4.gsm";
unlink "/tmp/linuxmce-security5.gsm";

$EXT_BUFFER .= "exten => s,n,Background(pluto/linuxmce-security-options)\n\n";

# Generating speech for SOS Menu Option - Alarm Reset
print "Generating speech for SOS Menu Option - Alarm Reset\n";
$list = "";
&generate_voice("The alarm has been reset.","/tmp/linuxmce-security-reset.gsm");
$list .= "/tmp/linuxmce-security-reset.gsm";

`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/linuxmce-security-reset.gsm`;

# Clean up
unlink "/tmp/linuxmce-security-reset.gsm";

$EXT_BUFFER .= "exten => 1,1,Playback(pluto/linuxmce-security-reset)\n";
$EXT_BUFFER .= "exten => 1,n,System(/usr/pluto/bin/RestartSecurityPlugin.sh)\n";
$EXT_BUFFER .= "exten => 1,n,Goto(s,3)\n\n";

# Generating speech for SOS Menu Option - Ignore
print "Generating speech for SOS Menu Option - Ignore\n";
$list = "";
&generate_voice("The alarm has been ignored.","/tmp/linuxmce-security-ignore.gsm");
$list .= "/tmp/linuxmce-security-ignore.gsm";

`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/linuxmce-security-ignore.gsm`;

# Clean up
unlink "/tmp/linuxmce-security-ignore.gsm";

$EXT_BUFFER .= "exten => 2,1,Playback(pluto/linuxmce-security-ignore)\n";
$EXT_BUFFER .= "exten => 2,n,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => 3,1,Goto(pluto-security-sos-custom,s,1)\n";
$EXT_BUFFER .= "exten => 3,n,Hangup\n\n";

$EXT_BUFFER .= "exten => 4,1,NoOp(\"Conference is performed with custom build addon (may not work)\")\n";
$EXT_BUFFER .= "exten => 4,n,System(/usr/pluto/bin/pluto-sos-neighbor.pl)\n";
$EXT_BUFFER .= "exten => 4,n,Dial(Local/997\@trusted/n)\n";
$EXT_BUFFER .= "exten => 4,n,Hangup\n\n";

$EXT_BUFFER .= "exten => 5,1,NoOp(\"Speak in the house\")\n";
$EXT_BUFFER .= "exten => 5,n,Dial(Local/998\@trusted/n)\n";
$EXT_BUFFER .= "exten => 5,n,Hangup\n\n";

$EXT_BUFFER .= "exten => *,1,Goto(linuxmce-userpin,s,1)\n\n";

$EXT_BUFFER .= "exten => #,1,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => t,1,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => i,1,Playback(pluto/invalid-entry)\n";
$EXT_BUFFER .= "exten => i,n,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => h,1,Hangup\n\n";

$EXT_BUFFER .= "[pluto-security-sos-custom]\n";
$EXT_BUFFER .= "exten => s,1,Answer\n";
$EXT_BUFFER .= "exten => s,n,Wait(1)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(digit)=14)\n";
$EXT_BUFFER .= "exten => s,n,Set(TIMEOUT(response)=5)\n";
$EXT_BUFFER .= "exten => s,n,Background(pluto/pluto-security-sos-options)\n\n";

# Insert the names we generated earlier
$EXT_BUFFER .= $TMP_BUFFER;

$EXT_BUFFER .= "exten => *,1,Goto(pluto-security-custom,s,1)\n";
$EXT_BUFFER .= "exten => *,n,Hangup\n\n";

$EXT_BUFFER .= "exten => #,1,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => t,1,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => i,1,Playback(pluto/invalid-entry)\n";
$EXT_BUFFER .= "exten => i,n,Goto(s,3)\n\n";

$EXT_BUFFER .= "exten => h,1,Hangup";

open(FILE,"> $EXT_FILE") or die "Could not open '$EXT_FILE'";
print FILE $EXT_BUFFER;
close(FILE);

$statement->finish();
$db_handle->disconnect();

sub generate_voice()
{
        &generate_voice_festival(@_);
}

sub generate_voice_festival()
{
        my $TEXT = shift;
        my $FILE = shift;
        my $DB_PL_HANDLE2;$db_handle = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";

        my $DB_STATEMENT2;
        my $DB_SQL2;

        #TODO: make proper mysql query to get default voice for voice generation! This is a bit of an ugly hack, but should work!
        $DB_SQL2 = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=283 LIMIT 1";
        $DB_STATEMENT2 = $db_handle->prepare($DB_SQL2) or die "Couldn't prepare query '$DB_SQL2': $DBI::errstr\n";
        $DB_STATEMENT2->execute() or die "Couldn't execute query '$DB_SQL2': $DBI::errstr\n";
        my $DB_ROW2 = $DB_STATEMENT2->fetchrow_hashref();
        my $defaultVoice=$DB_ROW2->{'IK_DeviceData'};
        $DB_STATEMENT2->finish();

        # Create a text file for festival to use
        open TF,">/tmp/festival.txt";
        print TF "$TEXT\n";
        close TF;

        # Create the wave file
        system('text2wave /tmp/festival.txt -o /tmp/festival.wav -eval "(' . $defaultVoice . ')"');

        # Resample and create permanent file
        `/usr/bin/sox /tmp/festival.wav -r 8000 -c 1 $FILE resample -ql`;

        # Clean up the temporary files
        unlink "/tmp/festival.wav";
        unlink "/tmp/festival.txt";
}
