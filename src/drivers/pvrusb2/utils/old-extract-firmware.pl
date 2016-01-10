#!/usr/bin/perl

use Fcntl;

my $sysfile = $ARGV[0]; # Give the path to HCWUSB2.SYS as argument

sub bindump {
    my $path = shift;
    my $data = shift;
    sysopen(DUMP, $path, O_CREAT|O_WRONLY|O_TRUNC, 00666)
	or die "Couldn't create $path: $!";
    my $n = syswrite(DUMP, $data);
    if (!defined($n) || $n != length($data)) {
	die "Couldn't write ".length($data)." bytes";
    }
    close(DUMP);
}

sysopen(SYS, $sysfile, O_RDONLY) or die "Couldn't open $sysfile: $!";

my $fx2pos = 03610700;		# 03573600 for the old pvrusb2_22_21234.exe

sysseek(SYS, $fx2pos, 0) or die "Couldn't seek to $fx2pos";
my $fx2buf = "";
my $fx2size = 8192;
my $n = sysread(SYS, $fx2buf, $fx2size);
if (!defined($n) || $n < $fx2size) {
    die "Couldn't read $fx2size bytes";
}

my $xxxpos = 00610700;		# 00573600 for the old pvrusb2_22_21234.exe

sysseek(SYS, $xxxpos, 0) or die "Couldn't seek to $xxxpos";
my $xxxbuf = "";
my $xxxsize = 262144;
$n = sysread(SYS, $xxxbuf, $xxxsize);
if (!defined($n) || $n < $xxxsize) {
    die "Couldn't read $xxxsize bytes";
}

close(SYS);

if (unpack("V", substr($fx2buf, 0011000+8, 4)) == 0x29002040 &&
    unpack("V", substr($xxxbuf, 4, 4)) == 0xaa55bb66) {
    &bindump("pvrusb2.f1", $fx2buf);
    &bindump("pvrusb2.f2", $xxxbuf);
} else {
    die "Extraction failed due to wrong magic number";
}

