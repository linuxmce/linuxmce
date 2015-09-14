#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
use v5.10.1;
use utf8;
no utf8;
 
require "/usr/pluto/bin/config_ops.pl";
require "/usr/pluto/bin/lmce.pl";

use vars ('$DCERouter','$PK_Device','$MySqlHost','$MySqlPort','$MySqlUser','$MySqlPassword','$MySqlDBName');

my $DB_PL_HANDLE;
my $DB_STATEMENT;
my $DB_SQL;
my $DB_ROW;
my $TTS_Device;
my $FK_Language;

$ENV{'LIA_PHON_REP'} = '/usr/share/mbrola/lia_phon'; 

$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";

# Get PK_Device of TTS device
$DB_SQL = "SELECT PK_Device FROM Device WHERE FK_DeviceTemplate=57 LIMIT 1";
$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
$DB_ROW = $DB_STATEMENT->fetchrow_hashref();
$TTS_Device=$DB_ROW->{'PK_Device'};
$DB_STATEMENT->finish();

# Get language for TTS
if (defined $TTS_Device && length $TTS_Device > 0) {
	$FK_Language = get_device_devicedata($TTS_Device,26);
}

# Fallback to english if no language defined
if ($FK_Language eq "") {
	$FK_Language = 1;
}

# Let's generate the IVR menu voice based on language. See below example:
#'Thank you for calling. To call everybody in the house, press 0. To call George, press 1. To call Mary, press 2. To leave a message, press #.';
my $list = "";
print "Generating speech for IVR Main Menu (language: $FK_Language).\n";
# PK_Text 2173 (Thank you for calling.)
&generate_voice(&get_text($FK_Language, 2173),"/tmp/pluto-default-voicemenu1.gsm");
# PK_Text 2168 (If you know the extension of the person you wish to call, you may enter it at any time.)
&generate_voice(&get_text($FK_Language, 2168),"/tmp/pluto-default-voicemenu2.gsm");
# PK_Text 2167 (To call everybody in the house, dial 0.)
&generate_voice(&get_text($FK_Language, 2167),"/tmp/pluto-default-voicemenu3.gsm");
# PK_Text 2169 (To leave a message, please press the pound sign.)
&generate_voice(&get_text($FK_Language, 2169),"/tmp/pluto-default-voicemenu4.gsm");

$list .= "/tmp/pluto-default-voicemenu1.gsm ";
$list .= "/tmp/pluto-default-voicemenu2.gsm ";
$list .= "/tmp/pluto-default-voicemenu3.gsm ";

$DB_SQL = "select if((Nickname=\"\" OR Nickname IS NULL),UserName,Nickname) AS Name, Extension from Users where `Extension` like '30%' and HasMailbox = 1"; 
$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
while(my $DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{
    my $i=$1 if($DB_ROW->{'Extension'} =~ /(\d)$/);
	my $j=$DB_ROW->{'Extension'};
	my $Text="";
    print "Generating speech for ".$DB_ROW->{'Name'}." extension: $j\n";
    # PK_Text 2170 (to call ), 2171 (, dial )
	$Text = &get_text($FK_Language, 2170).$DB_ROW->{'Name'}.&get_text($FK_Language, 2171).$i.".";
	print "		Result: $Text\n";
    &generate_voice($Text,"/tmp/pluto-default-voicemenu3-$i.gsm");
    $list .= "/tmp/pluto-default-voicemenu3-$i.gsm ";
	`/bin/mkdir -p /var/spool/asterisk/voicemail/default/$j/INBOX/Old`;
    &generate_voice($DB_ROW->{'Name'},"/var/spool/asterisk/voicemail/default/$j/greet.gsm");
}
$DB_STATEMENT->finish();

$list .= "/tmp/pluto-default-voicemenu4.gsm";

`/bin/mkdir -p /usr/share/asterisk/sounds/pluto`;
`/bin/chmod 770 -R /usr/share/asterisk/sounds/pluto`;
`/usr/bin/sox $list /usr/share/asterisk/sounds/pluto/pluto-default-voicemenu.gsm`;
`/bin/chown -R asterisk:www-data /usr/share/asterisk/sounds/pluto`;
`/bin/mkdir -p /var/spool/asterisk/voicemail/default/`;
`/bin/chown -R asterisk:www-data /var/spool/asterisk/voicemail`;
`/bin/chmod 755 -R /var/spool/asterisk`;
`/bin/chmod 770 -R /var/spool/asterisk/voicemail`;

# Clear $list and generate voice for invalid IVR entries
$list = "";

# Let's generate the voice for invalid IVR entries.
print "Generating speech for invalid IVR entries.\n";
# PK_Text 2172
&generate_voice(&get_text($FK_Language, 2172),"/tmp/invalid-entry.gsm");
$list .= "/tmp/invalid-entry.gsm ";
`/usr/bin/sox $list /usr/share/asterisk/sounds/pluto/invalid-entry.gsm`;
`/bin/chown -R asterisk:www-data /usr/share/asterisk/sounds/pluto`;

# Clean up
unlink "/tmp/pluto-default-voicemenu1.gsm";
unlink "/tmp/pluto-default-voicemenu2.gsm";
unlink "/tmp/pluto-default-voicemenu3.gsm";
unlink "/tmp/pluto-default-voicemenu4.gsm";
unlink "/tmp/invalid-entry.gsm";

# Get text for given language. Fallback to english if no translated text available.
sub get_text()
{
	my ($FK_Language, $FK_Text) = @_;
	my $Text;
	my $SQL;
	my $STATEMENT;
	my $ROW;
	
	$SQL = "SELECT Description FROM Text_LS WHERE FK_Language=$FK_Language AND FK_Text=$FK_Text LIMIT 1";
	$STATEMENT = $DB_PL_HANDLE->prepare($SQL) or die "Couldn't prepare query '$SQL': $DBI::errstr\n";
	$STATEMENT->execute() or die "Couldn't execute query '$SQL': $DBI::errstr\n";
	$ROW = $STATEMENT->fetchrow_hashref();
	$Text=$ROW->{'Description'};
	$STATEMENT->finish();
	
	# If not found, try to load english test from Text_LS (unlimited length)
	if(!defined($Text)) { 
		$SQL = "SELECT Description FROM Text_LS WHERE FK_Language=1 AND FK_Text=$FK_Text LIMIT 1";
		$STATEMENT = $DB_PL_HANDLE->prepare($SQL) or die "Couldn't prepare query '$SQL': $DBI::errstr\n";
		$STATEMENT->execute() or die "Couldn't execute query '$SQL': $DBI::errstr\n";
		$ROW = $STATEMENT->fetchrow_hashref();
		$Text=$ROW->{'Description'};
		$STATEMENT->finish();
	}
	
	# If not found, revert to original text in TEXT table (limited to 40 characters)
	if(!defined($Text)) { 
		$Text=""; 
		$SQL = "SELECT Description FROM Text WHERE PK_Text=$FK_Text LIMIT 1";
		$STATEMENT = $DB_PL_HANDLE->prepare($SQL) or die "Couldn't prepare query '$SQL': $DBI::errstr\n";
		$STATEMENT->execute() or die "Couldn't execute query '$SQL': $DBI::errstr\n";
		$ROW = $STATEMENT->fetchrow_hashref();
		$Text=$ROW->{'Description'};
		$STATEMENT->finish();
	}
	
	# If still not defined return error
	if(!defined($Text)) { $Text="Error during voice creation"; }
	
	utf8::decode($Text);
	print "		Text $FK_Text: $Text\n";
	return $Text;
}

sub generate_voice()
{
	given($FK_Language) {
		# french
		when(2) { &generate_voice_mbrola(@_); }
		# german
		when(3) { &generate_voice_mbrola(@_); }
		#portuguese (brazilian)
		when(11) { &generate_voice_googletts(@_, 'pt-br'); }
		# english and everything else :-)
		default { &generate_voice_festival(@_); }
	}
}

sub generate_voice_festival()
{
	my $TEXT = shift;
	my $FILE = shift;
	my $DB_PL_HANDLE2;$DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";

	my $DB_STATEMENT2;
	my $DB_SQL2;

	my $defaultVoice = get_device_devicedata(19,283);
	
	# Create a text file for festival to use
	open TF,">/tmp/festival.txt";
	print TF "$TEXT\n";
	close TF;

	# Create the wave file
	system('text2wave /tmp/festival.txt -o /tmp/festival.wav -eval "(' . $defaultVoice . ')"');

	# Resample and create permanent file
	`/usr/bin/sox /tmp/festival.wav -r 8000 -c1 $FILE`;

	# Clean up the temporary files
	unlink "/tmp/festival.wav";
	unlink "/tmp/festival.txt";
}

sub generate_voice_mbrola()
{
	my $TEXT = shift;
	my $FILE = shift;
	
	given ($FK_Language) {
		# french
		when(2) {
			system('echo "'.$TEXT.'" | espeak -v mb-fr4 -w /tmp/tts.wav');
		}
		# german
		when(3) {
			system('echo "'.$TEXT.'" | espeak -v mb-de7 -w /tmp/tts.wav');
		}
	}

	# Resample and create permanent file
	`/usr/bin/sox /tmp/tts.wav -r 8000 -c1 $FILE`;
	unlink "/tmp/tts.wav";
}

sub generate_voice_googletts()
{
	my $TEXT = shift;
	my $FILE = shift;
	my $LANGUAGE = shift;

	system('/usr/pluto/bin/googletts-cli.pl -r 8000 -o '.$FILE.'  -l '.$LANGUAGE.' -t "'.$TEXT.'"');
}

# `/usr/pluto/bin/db_phone_config.sh`;
`/usr/pluto/bin/db_create_dialplan.sh`;
