#!/usr/bin/perl
# $Id: usbreplayprep.pl 499 2005-07-16 20:36:34Z isely $
# usbreplayprep.pl
# This perl script reads an usbsnoop log file from STDIN and extracts
# commands and output data buffers, and prints them out like this:
#
# w1 39
#     00000000: 01 00 00 00 00 00 00 4c 00 00 00 00 00 00 4d 00
#     00000010: 00 00 00 00 00 4e 00 00 00 00 00 00 4f 00 00 00
#     00000020: 00 00 00 50 00 00 00 00 00 00 51 00 00 00 00 00
#     00000030: 00 52 00 00 00 00 00 00 53
#
# r81 40
#
# The "w1 39" section means write a buffer of size 0x39 to Endpoint 1,
# and "r81 40" means read 0x40 bytes from Endpoint 0x81.
#
# I have only tried this program with logs from usbsnoop-1.8 and
# Windows XP. It probably needs adjustments for other cases.
#
# The USB sniffer can be found here:
#   http://benoit.papillault.free.fr/usbsnoop/
#

use strict;

my %sentUrbs;
my $processedCount = 0;

sub print_bufstuff {
    my $text = shift;
    my ($t1,$t2);
    if ($text =~ m/TransferBufferMDL.*\n((.|\n)*)$/) {
	$t1 = $1;
	if ($t1 =~ m/^((.|\n)*)  UrbLink/) {
	    $t2 = $1;

# We can't syntax check the actual hex, because Perl explodes with a
# SEGV on *really* long strings (like big blocks of video data).  So
# we really have no choice but to trust that it is correct.  If we
# really want to be anal here, we can break this apart into lines and
# check each piece, but I don't think it's worth the effort.
#	    if ($t2 =~ m/^((    [0-9a-f]{8}:( [0-9a-f]{2})+\n)+)$/) {
		print $1;
#	    }
	}
    }

# This won't work on really long strings (see above rant)
#    if ($text =~ m/TransferBufferMDL.*\n((    [0-9a-f]{8}:( [0-9a-f]{2})+\n)+)  UrbLink/) {
#        print $1;
#    }
}

sub process_urb {
    my $urbText = shift;
    my $lineNum = shift;
    my ($urbOut,$urbIn);
    my ($urbLineIn,$urbLineOut);
    my $urbNum;

    return if (!defined $urbText);
    return if (!($urbText =~ m/URB (\d+) (going down|coming back)/));

    $urbNum = $1;
    if ($2 eq 'going down') {
	if (exists $sentUrbs{$urbNum}) {
	    print STDERR "Ignoring duplicate outgoing URB $urbNum at line $lineNum, previously at line $sentUrbs{$urbNum}->{lineNum}\n";
	    return;
	}
	$sentUrbs{$urbNum} = {urb => $urbText, lineNum => $lineNum};
	return;
    }

    if (! exists $sentUrbs{$urbNum}) {
	print STDERR "Ignoring anomalous incoming URB $urbNum at line $lineNum with no outgoing counterpart\n";
	return;
    }

    $urbIn = $urbText;
    $urbLineIn = $lineNum;
    $urbOut = $sentUrbs{$urbNum}->{urb};
    $urbLineOut = $sentUrbs{$urbNum}->{lineNum};
    delete $sentUrbs{$urbNum};

    if ($urbOut =~ m/PipeHandle.*endpoint 0x([0-9a-f]+)/) {
	my $endpoint = hex($1);
	if ($urbOut =~ m/TransferBufferLength\s+=\s+([0-9a-f]+)/) {
	    my $bufsize = hex($1);
	    if ($urbOut =~ m/TransferFlags.*_TRANSFER_DIRECTION_IN/) {
#		print "# Line $urbLineOut\n";
		printf "r%x %x\n", $endpoint, $bufsize;
		print_bufstuff($urbIn);
	    } elsif ($urbOut =~ m/TransferFlags.*_TRANSFER_DIRECTION_OUT/) {
#		print "# Line $urbLineOut\n";
		printf "w%x %x\n", $endpoint, $bufsize;
		print_bufstuff($urbOut);
	    }
	    $processedCount++;
	}
    }
}

# For debugging
#select(STDOUT);
#$| = 1;

my ($line,$urb);
my $lineNum = 0;
my $lineCNum = undef;

while (defined($line = <STDIN>)) {
    $lineNum++;
    if (! defined $lineCNum) {
	$lineCNum = $lineNum;
    }
    if ($line =~ m/ URB (\d+) (going down|coming back)/) {
        process_urb($urb,$lineCNum);
	$lineCNum = undef;
        $urb = $line;
    } elsif (defined($urb)) {
        $urb .= $line;
    }
}

process_urb($urb,$lineCNum);

my $key;

foreach $key (sort keys %sentUrbs) {
    print STDERR "Unanswered URB $key at line $sentUrbs{$key}->{lineNum}\n";
}

print STDERR "Processed $processedCount URBs in $lineNum lines of usbsnoop log data\n";
