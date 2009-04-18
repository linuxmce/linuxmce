#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

my $DB_PL_HANDLE;
my $DB_STATEMENT;
my $DB_SQL;

$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";

#Lets generate the IVR menu voice. See below example:
#'Thank you for calling. To call everybody in the house, press 0. To call George, press 1. To call Mary, press 2. To leave a message, press #.';
print "Generating speech for IVR Main Menu.\n";
my $list = "";
&generate_voice("Thank you for calling.","/tmp/pluto-default-voicemenu1.gsm");
$list .= "/tmp/pluto-default-voicemenu1.gsm ";
&generate_voice("If you know the extension of the person you wish to call, you may enter it at any time.","/tmp/pluto-default-voicemenu2.gsm");
$list .= "/tmp/pluto-default-voicemenu2.gsm ";
&generate_voice("To call everybody in the house, dial 0.","/tmp/pluto-default-voicemenu3.gsm");
$list .= "/tmp/pluto-default-voicemenu3.gsm ";
&generate_voice("To leave a message, please press the pound sign.","/tmp/pluto-default-voicemenu4.gsm");
#$list .= "/tmp/pluto-default-voicemenu4.gsm "; #we will add this to the list last, after all user options are generated.
#`/usr/bin/sox $list /tmp/pluto-default-voicemenu.gsm`;


#$list = "";
#$list .= "/var/lib/asterisk/sounds/pluto/pluto-default-voicemenu1.gsm ";
#$list .= "/var/lib/asterisk/sounds/pluto/pluto-default-voicemenu2.gsm ";

$DB_SQL = "select if(Nickname=\"\",UserName,Nickname) AS Name, Extension from Users where `Extension` like '30%' and HasMailbox = 1";
$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
while(my $DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{
    my $i=$1 if($DB_ROW->{'Extension'} =~ /(\d)$/);
	my $j=$DB_ROW->{'Extension'};
    print "Generating speech for ".$DB_ROW->{'Name'}." extension: $j\n";
    &generate_voice("To call ".$DB_ROW->{'Name'}.", dial ".$i.".","/tmp/pluto-default-voicemenu3-$i.gsm");
    $list .= "/tmp/pluto-default-voicemenu3-$i.gsm ";
	`/bin/mkdir -p /var/spool/asterisk/voicemail/default/$j/INBOX/Old`;
    &generate_voice($DB_ROW->{'Name'},"/var/spool/asterisk/voicemail/default/$j/greet.gsm");
}
$DB_STATEMENT->finish();

$list .= "/tmp/pluto-default-voicemenu4.gsm";
`/usr/bin/sox $list /var/lib/asterisk/sounds/pluto/pluto-default-voicemenu.gsm`;
`/bin/mkdir -p /var/spool/asterisk/voicemail/default/`
`/bin/chown -R asterisk:www-data /var/spool/asterisk/voicemail/*`;
`/bin/chmod 770 -R /var/spool/asterisk/voicemail/*`;

#clean up
unlink "/tmp/pluto-default-voicemenu1.gsm";
unlink "/tmp/pluto-default-voicemenu2.gsm";
unlink "/tmp/pluto-default-voicemenu3.gsm";
unlink "/tmp/pluto-default-voicemenu4.gsm";
sub generate_voice()
{
	&generate_voice_festival(@_);
}

sub generate_voice_att()
{
    my $SERVER = 'http://192.20.225.55';
    my $URL_FORM = '/tts/cgi-bin/nph-talk';
    my $URL_SOUND = "";
    my %URL_VARS = ();
    my $URL_DATA = "";
    my $TEXT = shift;
    my $FILE = shift;

    `rm -f /tmp/curl.*`;

    $URL_VARS{'voice'}='crystal';
    $URL_VARS{'txt'}=$TEXT;
    $URL_VARS{'downloadButton'}='DOWNLOAD';

    foreach my $var (keys %URL_VARS)
    {
        my $str = $URL_VARS{$var};
        $str =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;
        $URL_DATA .= $var."=".$str."&";
    }
    `curl -d '$URL_DATA' '$SERVER$URL_FORM' > /tmp/curl.log`;
    open(PAGE,"/tmp/curl.log") or die "Bad thing happend";
    while(<PAGE>)
    {
        chomp;
        if($_ =~ /[<]A\sHREF[=]["]([^"]+)["][>]/)
        {
            $URL_SOUND=$1;
        }
    }
    close(PAGE);
    `curl '$SERVER$URL_SOUND' > /tmp/curl.wav`;
    `/usr/bin/sox /tmp/curl.wav -r 8000 -c 1 $FILE resample -ql`;
}

sub generate_voice_festival()
{
	my $TEXT = shift;
	my $FILE = shift;
	my $DB_PL_HANDLE2;$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";

	my $DB_STATEMENT2;
	my $DB_SQL2;

	#TODO: make proper mysql query to get default voice for voice generation! This is a bit of an ugly hack, but should work!
	$DB_SQL2 = "SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_DeviceData=283 LIMIT 1";
	$DB_STATEMENT2 = $DB_PL_HANDLE->prepare($DB_SQL2) or die "Couldn't prepare query '$DB_SQL2': $DBI::errstr\n";
	$DB_STATEMENT2->execute() or die "Couldn't execute query '$DB_SQL2': $DBI::errstr\n";
	my $DB_ROW2 = $DB_STATEMENT2->fetchrow_hashref();
	my $defaultVoice=$DB_ROW2->{'IK_DeviceData'};
	$DB_STATEMENT2->finish();

	#create a text file for festival to use
	open TF,">/tmp/festival.txt";
	print TF "$TEXT\n";
	close TF;
	#Create the wave file
	system('text2wave /tmp/festival.txt -o /tmp/festival.wav -eval "(' . $defaultVoice . ')"');
	#resample and create permanent file
	`/usr/bin/sox /tmp/festival.wav -r 8000 -c 1 $FILE resample -ql`;
	#clean up the temporary files
	unlink "/tmp/festival.wav";
	unlink "/tmp/festival.txt";
}

#`/usr/pluto/bin/create_telecom_defaults.pl`;
`/usr/pluto/bin/create_pluto_dialplan.pl`;
