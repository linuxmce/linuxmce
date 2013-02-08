#!/usr/bin/perl

use DBI;
use Socket;
require "/usr/pluto/bin/config_ops.pl";

$db = DBI->connect(&read_pluto_cred()) or die "Couldn't connect to database: $DBI::errstr\n";
$gw = "";

`rm -f /var/log/pluto/gc100-conf.log >> /dev/null`;

if($ARGV[0] eq "") {
    loggc("No params\n\tFinding GC100 on the network\n");
    $flag = allias_up();
    if(find_gc100() == 1) {
        loggc("GC100 Found\n");
	$mac = get_gc100mac();
	loggc("MAC Address: $mac\n");
	loggc("Finding Installation ");
	$install = get_install();
	loggc("[$install]\n");
	loggc("Finding Template ");
	$dev_templ = get_template();
	loggc("[$dev_templ]\n");
	$sa = "select * from Device where FK_DeviceTemplate='$dev_templ'";
	$state = $db->prepare($sa) or die "first";
	$state->execute() or die "second";
	while($row_x = $state->fetchrow_hashref()) {
		$newmac = $row_x->{'MACaddress'};
		$ret = comp_mac($newmac,$mac);
		if($ret == 1) {
			loggc("The device already exist in the database\n");
			$state->finish();
			$db->disconnect();
			exit(2);
		}
	}
	$state->finish();
	# Send "Create Device" (718) message to General Info Plugin (template 27)
	$cmd = "/usr/pluto/bin/MessageSend $DCERouter -o -targetType template 0 27 1 718 44 $dev_templ 57 -1 47 $mac 58 auto 198 0 156 $PK_Device";
	loggc("$cmd\n");
	loggc("Creating Device...");
	$Device_ID = `$cmd`;
	@lines = split(/\n/s, $Device_ID);
	foreach $line (@lines)
	{
		print "D: $line\n";
		if ($line =~ /^2:/)
		{
			$Device_ID = $line;
			$Device_ID =~ s/^2://;
			last;
		}
	}
	chomp($Device_ID);
	print "$Device_ID\n";	
	loggc(" [$Device_ID]\n");
	
	loggc("Finding IP ");
	$ip = find_ip($Device_ID);
	loggc("[$ip]\n");
	loggc("Configuring GC100 via Web...");
	configure_webgc($ip);
	loggc("Done\n");
	if($flag == 1) {
	    allias_down();
	}
    } else {
        loggc("Gc100 as default not found\n");
	exit(1);
    }
} else {
exit(0);
    loggc("With params\nFinding GC100 on the network\n");
    if($ARGV[1] eq "") {
        if(find_gc100() == 1) {
            loggc("GC100 found\n");
    	    $flag = allias_up();
	    $mac = get_gc100mac();
	    $ip = find_ip(-1);
	} else {
	    loggc("Gc100 as default not found\n");
	    exit(1);
	}
    }
    $ip = $ARGV[1];
    $flag = allias_up();
    configure_webgc($ip);
    if($flag == 1) {
        allias_down();
    }
}

$db->disconnect();

sub comp_mac {
	$fst = shift;
	$snd = shift;
	@f1 = split(/\:/,$fst);
	@f2 = split(/\:/,$snd);
	print "@f1\n";
	print "@f2\n---\n";
	if(hex($f1[0]) eq hex($f2[0]) && hex($f1[1]) eq hex($f2[1]) && hex($f1[2]) eq hex($f2[2]) && hex($f1[3]) eq hex($f2[3]) && hex($f1[4]) eq hex($f2[4]) && hex($f1[5]) eq hex($f2[5])) {
		return 1;
	} else {
		return 0;
	}
}

sub ip_2_long {
  $local_ip = shift;
  $local_long = "";
  @frag = split(/\./,$local_ip);
  for($i=0;$i<4;$i=$i+1) {
    if($frag[$i] < 10) {
      $frag[$i] = "00".$frag[$i];
    } elsif($frag[$i] < 100) {
      $frag[$i] = "0".$frag[$i];
    }
  }
  return $frag[0].$frag[1].$frag[2].$frag[3];
}

sub long_2_ip {
  $local_long = shift;
  $start = 0;
  for($i=0;$i<4;$i=$i+1) {
    $dat[$i] = substr($local_long,$start,3);
    $dat[$i] = $dat[$i] / 1;
    $start = $start + 3;
  }
  
  return $dat[0].".".$dat[1].".".$dat[2].".".$dat[3];
}

sub sqlexec {
  $local_line = shift;
  $st = $db->prepare($local_line);
  $st->execute();
  $i=0;
  while($local_row = $st->fetchrow_hashref()) {
    $outdata[$i]=$local_row;
    $i = $i + 1;
  }         
  $st->finish();
  return @outdata;
}

sub get_install {
  open(FILE,"/etc/pluto.conf");
  @data=<FILE>;
  close(FILE);
  foreach $line (@data) {
    chomp($line);
    ($var,$value) = split(/=/,$line);
    @frag = split(/ /,$var);
    $var = $frag[0];
    @frag = split(/ /,$value);
    $value = $frag[1];
    if($var eq "PK_Installation") {
      $sql = "select FK_Installation from Device WHERE PK_Device='$PK_Device'";
      $st = $db->prepare($sql);
      $st->execute();
      if($row = $st->fetchrow_hashref()) {
        return $row->{'FK_Installation'};
      } else {
        return 1;
      }
      $st->finish();
    }
  }
}

sub get_template {
  $sql = "select PK_DeviceTemplate from DeviceTemplate WHERE Description='gc100'";
  $st = $db->prepare($sql);
  $st->execute();
  if($row = $st->fetchrow_hashref()) {
	$st->finish();
	return $row->{'PK_DeviceTemplate'};
  } else {
  	loggc("Invalid Template");
    	exit(4);
  }
}

sub get_gc100mac {
  `curl http://192.168.1.70/Commands.cgi -o gc100mac1 --silent`;
  `curl http://192.168.1.101/Commands.cgi -o gc100mac2 --silent`;
  open(FILE,"gc100mac1");
  @data1 = <FILE>;
  close(FILE);
  open(FILE,"gc100mac2");
  @data2 = <FILE>;
  close(FILE);
  $line1 = $data1[0];
  $line2 = $data2[0];
  @vars1 = split(/ /,$line1);
  @vars2 = split(/ /,$line2);
  @vars = (@vars1, @vars2);
  $i = 0;
  foreach $line (@vars) {
    if($vars[$i] eq "Address") {
	$ii = $i - 1;
	if($vars[$ii] eq "MAC") {
	    $j = $i + 2;
	    @frag = split(/\</,$vars[$j]);
	    $mac = $frag[0];
	    $mac =~ tr/\-/\:/;
	    loggc("Mac Found $mac\n");
	    system("rm -f gc100mac1 gc100mac2");
	    if($mac eq "") {
	      exit(1);
	    }
	    chop ($mac) if (substr($mac, length($mac) - 1, 1) == '\0');
	    return $mac;
	}
    }
    $i = $i + 1;
  }
}

sub allias_up {
    $sql = "select * from Device_DeviceData where FK_DeviceData='32'";
    $st = $db->prepare($sql);
    $st->execute();
    if($row = $st->fetchrow_hashref()) {
	$data = $row->{'IK_DeviceData'};
	@frag = split(/\|/,$data);
	@frag = split(/\,/,$frag[1]);
	$local_dev = $frag[0];
	($fst, $nd) = split(/\:/,$frag[0]);
	$local_dev = $fst;
	$local_ip = $frag[1];
    } else {
	$local_dev = "eth1"
    }
    $st->finish();
    
    $gw = $local_ip;
    loggc("device:$local_dev ip:$local_ip gw:$gw\n");
    @frag = split(/\./,$local_ip);
    if($frag[0] ne "192" || $frag[1] ne "168" || $frag[2] ne "1") {
        loggc("Making allias\n");
	system("ifconfig $local_dev:100 192.168.1.1 broadcast 255.255.255.255 netmask 255.255.255.0");
	return 1;
    } else {
	return 0;
    }
}

sub allias_down {
  	loggc("Bringing down the allias...");
  `ifconfig $local_dev:100 down`;
  	loggc("Down\n");
}

sub find_gc100 {
	loggc("Finding...\n");
    $host = shift || '192.168.1.70';
    $port = shift || 80;

    $proto = getprotobyname('tcp');

    $iaddr = inet_aton($host);
    $paddr = sockaddr_in($port, $iaddr); 
	loggc("Creating socket...");
    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
    	loggc("Done\n");
    $var = 0;
	loggc("Trying first ip...");
    connect(SOCKET, $paddr) or $var=1;
    if($var == 0) {
        close SOCKET;
	loggc("Found on first default ip\n");
	return 1;
    }
    loggc("Not found on first ip\n");
    
    $host = shift || '192.168.1.101';
    $port = shift || 80;
           
    $proto = getprotobyname('tcp');
                
    $iaddr = inet_aton($host);
    $paddr = sockaddr_in($port, $iaddr);
                        
    socket(SOCKET, PF_INET, SOCK_STREAM, $proto) or print "socket: $!";
    $varr = 0;
	loggc("Trying second ip...");
    connect(SOCKET, $paddr) or $varr=1;
    if($varr == 0) {
    	loggc("Found on second ip\n");
      close SOCKET;
      return 1;
    }
    	loggc("Not found on second ip\n");
    
    if($var == 1 && $varr == 1) {
      return 0;
    }
}

sub find_ip {
	my ($PKDEV) = shift;
	print "/usr/pluto/bin/PlutoDHCP.sh -d $PKDEV -a";
	$main_ip = `/usr/pluto/bin/PlutoDHCP.sh -d $PKDEV -a`;
	chomp($main_ip);
	return $main_ip;
}

sub configure_webgc {
    $local_ip = shift;
    system("wget -q -T 3 --read-timeout=4 -t 1 \"http://192.168.1.70/commands.cgi?2=$local_ip&3=255.255.255.0&4=$gw&7=submit\"");
    system("wget -q -T 3 --read-timeout=4 -t 1 \"http://192.168.1.101/commands.cgi?2=$local_ip&3=255.255.255.0&4=$gw&7=submit\"");
}

sub loggc {
    $line = shift;
    open(LOG,">>/var/log/pluto/gc100-conf.log");
    print LOG $line;
    print $line;
    close(LOG);
}
