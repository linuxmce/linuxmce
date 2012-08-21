#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";

sub getIP {
        my $dbh = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
        my $sth = $dbh->prepare("SELECT IPaddress FROM Device WHERE FK_DeviceTemplate = 7");
        $sth->execute || die "Sql Error";
        my $row = $sth->fetchrow_hashref;
        my $IP = $row->{IPaddress};
        return $IP;
}

sub getTimeZone {
        my $dbh = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
        my $sth = $dbh->prepare("SELECT ZoneName FROM TimeZone Where PK_Timezone = (select FK_Timezone from City Where PK_City = (Select FK_City From Installation))");
        $sth->execute || die "Sql Error";
        my $row = $sth->fetchrow_hashref;
        my $TimeZone = $row->{ZoneName};
        return $TimeZone;
}  

#declare vars (it's safer this way)
my $Device_ID;
my $Device_IP;
my $Device_MAC;
my $Device_EXT;
my $IntIP;

#check params
if($#ARGV < 5 || $ARGV[0] ne "-d" || $ARGV[2] ne "-i" || $ARGV[4] ne "-m")
{
    print "<USAGE-1>\n$0 -d <Device ID> -i <IP> -m <mac address>\n";
    exit(-1);
}
else
{
    $Device_ID = $ARGV[1];
    $Device_IP = $ARGV[3];
    $Device_MAC = $ARGV[5];
}

$IntIP = getIP();
if ($IntIP eq "") {
        $IntIP="192.168.80.1";
}

#sync with asterisk DB (practically do nothing but create a new extension number)
#`/usr/pluto/bin/db_phone_config.sh`;

open(FILE,"/tmp/phone${Device_ID}extension");
$Device_EXT=<FILE>;
close(FILE);

chomp($Device_EXT);
$Device_MAC =~ s/[^0-9A-Fa-f]//g;
$Device_MAC = uc($Device_MAC);

my $str_XMLDefault = "";
my $str_SEPDefault = "";
my $str_DEV = "";
my $str_LINE = "";
my $str_FILE = "";
my $str_TimeZone = getTimeZone();

$str_XMLDefault .= "<?xml version=\"1.0\"?>\n";
$str_XMLDefault .= "<Default>\n";
$str_XMLDefault .= "\t<callManagerGroup>\n";
$str_XMLDefault .= "\t\t<members>\n";
$str_XMLDefault .= "\t\t\t<member priority=\"0\">\n";
$str_XMLDefault .= "\t\t\t\t<callManager>\n";
$str_XMLDefault .= "\t\t\t\t\t<ports>\n";
$str_XMLDefault .= "\t\t\t\t\t\t<ethernetPhonePort>2000</ethernetPhonePort>\n";
$str_XMLDefault .= "\t\t\t\t\t</ports>\n";
$str_XMLDefault .= "\t\t\t\t\t<processNodeName>$IntIP</processNodeName>\n";
$str_XMLDefault .= "\t\t\t\t</callManager>\n";
$str_XMLDefault .= "\t\t\t</member>\n";
$str_XMLDefault .= "\t\t</members>\n";
$str_XMLDefault .= "\t</callManagerGroup>\n";
$str_XMLDefault .= "\t<servicesURL>http://$IntIP/lmce-admin/ServicesMenu.php</servicesURL>\n";
$str_XMLDefault .= "</Default>\n";

open(FILE,"> /tftpboot/XmlDefault.cnf.xml");
print FILE $str_XMLDefault;
close(FILE);

$str_SEPDefault .= "<device>\r\n";
$str_SEPDefault .= " <devicePool>\r\n";
$str_SEPDefault .= "  <callManagerGroup>\r\n";
$str_SEPDefault .= "   <members>\r\n";
$str_SEPDefault .= '    <member  priority="0">\r\n';
$str_SEPDefault .= "     <callManager>\r\n";
$str_SEPDefault .= "      <ports>\r\n";
$str_SEPDefault .= "       <ethernetPhonePort>2000</ethernetPhonePort>\r\n";
$str_SEPDefault .= "      </ports>\r\n";
$str_SEPDefault .= "      <processNodeName>192.168.80.1</processNodeName>\r\n";
$str_SEPDefault .= "     </callManager>\r\n";
$str_SEPDefault .= "    </member>\r\n";
$str_SEPDefault .= "   </members>\r\n";
$str_SEPDefault .= "  </callManagerGroup>\r\n";
$str_SEPDefault .= " </devicePool>\r\n";
$str_SEPDefault .= " <versionStamp>{Jan 01 2002 00:00:00}</versionStamp>\r\n";
$str_SEPDefault .= " <loadInformation></loadInformation>\r\n";
$str_SEPDefault .= " <userLocale>\r\n";
$str_SEPDefault .= "  <name>English_United_States</name>\r\n";
$str_SEPDefault .= "  <langCode>en</langCode>\r\n";
$str_SEPDefault .= " </userLocale>\r\n";
$str_SEPDefault .= " <networkLocale>United_States</networkLocale>\r\n";
$str_SEPDefault .= " <idleTimeout>0</idleTimeout>\r\n";
$str_SEPDefault .= " <authenticationURL>http://192.168.80.1/lmce-admin/authenticate_cisco.php</authenticationURL>\r\n";
$str_SEPDefault .= " <directoryURL>http://192.168.80.1/lmce-admin/cisco-xml-phone-directory/directory.php</directoryURL>\r\n";
$str_SEPDefault .= " <idleURL></idleURL>\r\n";
$str_SEPDefault .= " <informationURL></informationURL>\r\n";
$str_SEPDefault .= " <messagesURL></messagesURL>\r\n";
$str_SEPDefault .= " <proxyServerURL></proxyServerURL>\r\n";
$str_SEPDefault .= " <servicesURL></servicesURL>\r\n";
$str_SEPDefault .= "</device>\r\n";




open(FILE,"> /tftpboot/SEPDefault7941.cnf.xml");
print FILE $str_SEPDefault;
close(FILE);

`ln -sf /tftpboot/SEPDefault7941.cnf.xml /tftpboot/SEP$Device_MAC.cnf.xml`;

# no longer needed
#`/usr/pluto/bin/GenerateSCCP.sh`;


#A HARD RELOAD FOR ASTERISK (no longer needed in realtime env.)
#exec("service asterisk restart");

                                