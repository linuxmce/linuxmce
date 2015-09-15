#!/usr/bin/perl

use strict;
use diagnostics;
use DBI;
require "/usr/pluto/bin/config_ops.pl";
require "/usr/pluto/bin/lmce.pl";

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
my $Device_FIRMWARE;
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

$IntIP = getCoreIP();
if ($IntIP eq "") {
        exit(-1);
}

$Device_EXT=$Device_EXT = get_device_devicedata($Device_ID,31);
$Device_FIRMWARE = get_device_devicedata($Device_ID,313);
$Device_FIRMWARE =~ s{.loads}{};

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
$str_SEPDefault .= "<devicePool>\r\n";
$str_SEPDefault .= "<name>Default</name>\r\n";
$str_SEPDefault .= "<dateTimeSetting>\r\n";
$str_SEPDefault .= "<name>CMLocal</name>\r\n";
$str_SEPDefault .= "<dateTemplate>D.M.Y</dateTemplate>\r\n";
$str_SEPDefault .= "<timeZone>$str_TimeZone</timeZone>\r\n";
$str_SEPDefault .= "</dateTimeSetting>\r\n";
$str_SEPDefault .= "<callManagerGroup>\r\n";
$str_SEPDefault .= "<members>\r\n";
$str_SEPDefault .= "<member priority=\"0\">\r\n";
$str_SEPDefault .= "<callManager>\r\n";
$str_SEPDefault .= "<ports>\r\n";
$str_SEPDefault .= "<analogAccessPort>2002</analogAccessPort>\r\n";
$str_SEPDefault .= "<digitalAccessPort>2001</digitalAccessPort>\r\n";
$str_SEPDefault .= "<ethernetPhonePort>2000</ethernetPhonePort>\r\n";
$str_SEPDefault .= "<mgcpPorts>\r\n";
$str_SEPDefault .= "<listen>2427</listen>\r\n";
$str_SEPDefault .= "<keepAlive>2428</keepAlive>\r\n";
$str_SEPDefault .= "</mgcpPorts>\r\n";
$str_SEPDefault .= "</ports>\r\n";
$str_SEPDefault .= "<processNodeName>$IntIP</processNodeName>\r\n";
$str_SEPDefault .= "</callManager>\r\n";
$str_SEPDefault .= "</member>\r\n";
$str_SEPDefault .= "<member  priority=\"1\">\r\n";
$str_SEPDefault .= "<callManager>\r\n";
$str_SEPDefault .= "<ports>\r\n";
$str_SEPDefault .= "<analogAccessPort>2002</analogAccessPort>\r\n";
$str_SEPDefault .= "<digitalAccessPort>2001</digitalAccessPort>\r\n";
$str_SEPDefault .= "<ethernetPhonePort>2000</ethernetPhonePort>\r\n";
$str_SEPDefault .= "<mgcpPorts>\r\n";
$str_SEPDefault .= "<listen>2427</listen>\r\n";
$str_SEPDefault .= "<keepAlive>2428</keepAlive>\r\n";
$str_SEPDefault .= "</mgcpPorts>\r\n";
$str_SEPDefault .= "</ports>\r\n";
$str_SEPDefault .= "<processNodeName>$IntIP</processNodeName>\r\n";
$str_SEPDefault .= "</callManager>\r\n";
$str_SEPDefault .= "</member>\r\n";
$str_SEPDefault .= "</members>\r\n";
$str_SEPDefault .= "</callManagerGroup>\r\n";
$str_SEPDefault .= "<srstInfo>\r\n";
$str_SEPDefault .= "<name>Disable</name>\r\n";
$str_SEPDefault .= "<srstOption>Disable</srstOption>\r\n";
$str_SEPDefault .= "<userModifiable>false</userModifiable>\r\n";
$str_SEPDefault .= "<ipAddr1></ipAddr1>\r\n";
$str_SEPDefault .= "<port1>2000</port1>\r\n";
$str_SEPDefault .= "<ipAddr2></ipAddr2>\r\n";
$str_SEPDefault .= "<port2>2000</port2>\r\n";
$str_SEPDefault .= "<ipAddr3></ipAddr3>\r\n";
$str_SEPDefault .= "<port3>2000</port3>\r\n";
$str_SEPDefault .= "<isSecure>false</isSecure>\r\n";
$str_SEPDefault .= "</srstInfo>\r\n";
$str_SEPDefault .= "<mlppDomainId>-1</mlppDomainId>\r\n";
$str_SEPDefault .= "<mlppIndicationStatus>Default</mlppIndicationStatus>\r\n";
$str_SEPDefault .= "<preemption>Default</preemption>\r\n";
$str_SEPDefault .= "<connectionMonitorDuration>120</connectionMonitorDuration>\r\n";
$str_SEPDefault .= "</devicePool>\r\n";
$str_SEPDefault .= "<loadInformation>";
if ($Device_FIRMWARE ne "") {
	$str_SEPDefault .= $Device_FIRMWARE;
}
$str_SEPDefault .= "</loadInformation>\r\n";
$str_SEPDefault .= "<vendorConfig>\r\n";
$str_SEPDefault .= " <disableSpeaker>false</disableSpeaker>\r\n";
$str_SEPDefault .= " <disableSpeakerAndHeadset>false</disableSpeakerAndHeadset>\r\n";
$str_SEPDefault .= " <forwardingDelay>1</forwardingDelay>\r\n";
$str_SEPDefault .= " <pcPort>0</pcPort>\r\n";
$str_SEPDefault .= " <settingsAccess>1</settingsAccess>\r\n";
$str_SEPDefault .= " <garp>0</garp>\r\n";
$str_SEPDefault .= " <voiceVlanAccess>0</voiceVlanAccess>\r\n";
$str_SEPDefault .= " <videoCapability>0</videoCapability>\r\n";
$str_SEPDefault .= " <autoSelectLineEnable>0</autoSelectLineEnable>\r\n";
$str_SEPDefault .= " <webAccess>0</webAccess>\r\n";
$str_SEPDefault .= " <daysDisplayNotActive></daysDisplayNotActive>\r\n";
$str_SEPDefault .= " <displayOnTime>09:00</displayOnTime>\r\n";
$str_SEPDefault .= " <displayOnDuration>10:00</displayOnDuration>\r\n";
$str_SEPDefault .= " <displayIdleTimeout>00:05</displayIdleTimeout>\r\n";
$str_SEPDefault .= " <spanToPCPort>1</spanToPCPort>\r\n";
$str_SEPDefault .= "</vendorConfig>\r\n";
$str_SEPDefault .= "<versionStamp></versionStamp>\r\n";
$str_SEPDefault .= "<userLocale>\r\n";
$str_SEPDefault .= "<name>English_United_States</name>\r\n";
$str_SEPDefault .= "<uid>1</uid>\r\n";
$str_SEPDefault .= "<langCode>en</langCode>\r\n";
$str_SEPDefault .= "<version>4.1(3)</version>\r\n";
$str_SEPDefault .= "<winCharSet>iso-8859-1</winCharSet>\r\n";
$str_SEPDefault .= "</userLocale>\r\n";
$str_SEPDefault .= "<networkLocale>United_States</networkLocale>\r\n";
$str_SEPDefault .= "<networkLocaleInfo>\r\n";
$str_SEPDefault .= "<name>United_States</name>\r\n";
$str_SEPDefault .= "<uid>64</uid>\r\n";
$str_SEPDefault .= "<version>4.1(3)</version>\r\n";
$str_SEPDefault .= "</networkLocaleInfo>\r\n";
$str_SEPDefault .= "<deviceSecurityMode>1</deviceSecurityMode>\r\n";
$str_SEPDefault .= "<idleTimeout>0</idleTimeout>\r\n";
$str_SEPDefault .= "<authenticationURL>http://$IntIP/lmce-admin/authenticate_cisco.php</authenticationURL>\r\n";
$str_SEPDefault .= "<directoryURL>http://$IntIP/lmce-admin/cisco-xml-phone-directory/directory.php</directoryURL>\r\n";
$str_SEPDefault .= "<idleURL></idleURL>\r\n";
$str_SEPDefault .= "<informationURL></informationURL>\r\n";
$str_SEPDefault .= "<messagesURL></messagesURL>\r\n";
$str_SEPDefault .= "<proxyServerURL></proxyServerURL>\r\n";
$str_SEPDefault .= "<servicesURL>http://$IntIP/lmce-admin/ServicesMenu.php</servicesURL>\r\n";
$str_SEPDefault .= "<dscpForCm2Dvce>96</dscpForCm2Dvce>\r\n";
$str_SEPDefault .= "<dscpForSCCPPhoneConfig>96</dscpForSCCPPhoneConfig>\r\n";
$str_SEPDefault .= "<dscpForSCCPPhoneServices>0</dscpForSCCPPhoneServices>\r\n";
$str_SEPDefault .= "<capfAuthMode>2</capfAuthMode>\r\n";
$str_SEPDefault .= "<capfList>\r\n";
$str_SEPDefault .= "<capf>\r\n";
$str_SEPDefault .= "<phonePort>3804</phonePort>\r\n";
$str_SEPDefault .= "<processNodeName>$IntIP</processNodeName>\r\n";
$str_SEPDefault .= "</capf>\r\n";
$str_SEPDefault .= "</capfList>\r\n";
$str_SEPDefault .= "</device>\r\n";

open(FILE,"> /tftpboot/SEPDefault7970.cnf.xml");
print FILE $str_SEPDefault;
close(FILE);

`ln -sf /tftpboot/SEPDefault7970.cnf.xml /tftpboot/SEP$Device_MAC.cnf.xml`;

### Update Cisco 7970 Orbiter
my $ORB_ID;
my $ORB_CNT;
my $DB_ROW;
my $DB_PL_HANDLE = DBI->connect(&read_pluto_cred()) or die "Can't connect to database: $DBI::errstr\n";
my $DB_SQL = "select count(PK_Device) from Device where FK_DeviceTemplate=1727";
my $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
if($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{
    $ORB_CNT = $DB_ROW->{'count(PK_Device)'};
    $DB_STATEMENT->finish();
}
$DB_SQL = "select PK_Device from Device where FK_DeviceTemplate=1727 and FK_Device_ControlledVia=$Device_ID";
$DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
$DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
if($DB_ROW = $DB_STATEMENT->fetchrow_hashref())
{
    $ORB_ID = $DB_ROW->{'PK_Device'};
    $DB_STATEMENT->finish();
    #update ip
    $DB_SQL = "update Device_DeviceData SET IK_DeviceData='$Device_IP' WHERE FK_Device='$ORB_ID' AND FK_DeviceData='118'";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->finish();
    #update port
    $DB_SQL = "update Device_DeviceData SET IK_DeviceData='".(3450+$ORB_CNT)."' WHERE FK_Device='$ORB_ID' AND FK_DeviceData='119'";
    $DB_STATEMENT = $DB_PL_HANDLE->prepare($DB_SQL) or die "Couldn't prepare query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->execute() or die "Couldn't execute query '$DB_SQL': $DBI::errstr\n";
    $DB_STATEMENT->finish();
}
$DB_PL_HANDLE->disconnect();

