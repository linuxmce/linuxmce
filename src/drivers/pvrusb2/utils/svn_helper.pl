#!/usr/bin/perl

# $Id: svn_helper.pl 2336 2009-10-11 22:00:39Z isely $

#
#  Copyright (C) 2006 Mike Isely <isely@pobox.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

# Utility script for manipulating / patching source trees

use strict;
use IO::File;
use IO::Dir;

my %symtab;
my @fileset;


# -------------------------------------------------------------------------


sub optSystem {
    my $dryRun = shift;
    my $cmd = shift;
    print "$cmd\n";
    system $cmd if (!$dryRun);
}


# -------------------------------------------------------------------------


sub write_file {
    my $fname = shift;
    my $data = shift;
    my $fh = new IO::File(">$fname")
	or die "Failed to open $fname for writing\n";
    my $line;

    foreach $line (@$data) {
	print $fh $line . "\n";
    }
}


sub read_file {
    my $fname = shift;
    my @data;
    my $line;
    my $fh = new IO::File("<$fname")
	or die "Failed to open $fname for reading\n";

    while ($line = <$fh>) {
	chomp $line;
	push @data,$line;
    }
    return \@data;
}


# -------------------------------------------------------------------------


sub cstrip_fixup_skip {
    my $stkitem = shift;
    $stkitem->{skip} = (($stkitem->{active} && $stkitem->{lskip}) ||
			$stkitem->{pskip});
}


sub cstrip_new_stack_item {
    my $cstk = shift;
    my $stkitem;
    $stkitem = { active => undef,
		 pskip => (scalar(@$cstk) && $cstk->[0]->{skip}),
		 lskip => undef,
		 skip => undef};
    cstrip_fixup_skip($stkitem);
#    print "Pushing stack item\n";
    unshift @$cstk,$stkitem;
    return $stkitem;
}


sub cstrip_dump_stack_item {
    my $stkitem = shift;
    print "Frame:";
    if ($stkitem->{active}) {
	print " active";
    } else {
	print " inactive";
    }
    if ($stkitem->{lskip}) {
	print " lskip";
    }
    if ($stkitem->{pskip}) {
	print " pskip";
    }
    if ($stkitem->{skip}) {
	print " skip";
    }
    print "\n";
}


sub cstrip_check_directive {
    my $symtab = shift;
    my $cmd = shift;
    my $expr = shift;
    my $cstk = shift;
    my $linenum = shift;
    my $stkitem;
    my $activefl;

    $expr =~ s/\/\/.*$//;
    $expr =~ s/\/\*.*$//;
    $expr =~ s/^\s+//;
    $expr =~ s/\s+$//;

#    print "Examining cmd=\"$cmd\" expr=\"$expr\" on line $linenum\n";

    if ($cmd eq 'if') {
	$stkitem = cstrip_new_stack_item($cstk);
	# ????? Need to parse generic conditional directive, but for
	# the moment it isn't important to our needs.
	return $stkitem->{active} ? undef : "";
    }

    if (($cmd eq 'ifdef') or ($cmd eq 'ifndef')) {
	$stkitem = cstrip_new_stack_item($cstk);
	if ((defined $symtab) && (exists $symtab->{$expr})) {
	    $stkitem->{active} = 1;
	    $stkitem->{lskip} = ($symtab->{$expr}->{defined} xor
				 ($cmd eq 'ifdef'));
	    cstrip_fixup_skip($stkitem);
	}
#	cstrip_dump_stack_item($stkitem);
	return $stkitem->{active} ? undef : "";
    }

    if ($cmd eq 'else') {
	$stkitem = $cstk->[0];
	if ($stkitem->{active}) {
	    $stkitem->{lskip} = ! $stkitem->{lskip};
	}
	cstrip_fixup_skip($stkitem);
#	cstrip_dump_stack_item($stkitem);
	return $stkitem->{active} ? undef : "";
    }

    if ($cmd eq 'endif') {
	$stkitem = $cstk->[0];
#	print "Popping stack item\n";
	shift @$cstk;
	return $stkitem->{active} ? undef : "";
    }

    return undef;
}


# Process a block of data, expanding out any preprocessor conditional
# compilation directives for which we know the expansion.
sub cstrip_data {
    my $symtab = shift;
    my $inctab = shift;
    my $idata = shift;
    my $line;
    my $linenum;
    my @cond_stack;
    my $res;
    my @odata;
    my $cnt = scalar(@$idata);
    my $idx;
    my $name;

    for ($idx = 0; $idx < $cnt; $idx++) {
	$line = $idata->[$idx];
	$linenum++;
	if ((defined $symtab) &&
	    ($line =~ /^#\s*((?:ifdef)|(?:ifndef)|(?:if)|(?:else)|(?:endif))\s*(.*)$/)) {
	    $res = cstrip_check_directive($symtab,$1,$2,\@cond_stack,$linenum);
	    next if (!defined $res);
	    $line = $res if ($res ne '');
	}
	if ((defined $inctab) && ($line =~ /^#\s*include\s*\"(.*)\".*$/)) {
	    $name = $1;
	    if (exists $inctab->{$name}) {
		next if (! defined $inctab->{$name});
	    }
	}
	next if (scalar(@cond_stack) && $cond_stack[0]->{skip});
	push @odata,$line;
    }

    return \@odata;
}


# -------------------------------------------------------------------------


# Strip down any instance of the Id keyword down to its unexpanded form
sub istrip_data {
    my $idata = shift;
    my @odata;
    my $line;

    foreach $line (@$idata) {
	if ($line =~ /\$Id:.+\$/) {
	    next;
#	    $line =~ s/(.*)\$(Id)[:].+\$(.*)/$1\$$2\$$3/g;
	}
	push @odata,$line;
    }

    return \@odata;
}


# -------------------------------------------------------------------------


sub dump_symtab {
    my $symtab = shift;
    my $key;
    my $item;

    print "Active symbol table:\n";
    foreach $key (sort keys %$symtab) {
	$item = $symtab->{$key};
	print "\"$key\":";
	if ($item->{defined}) {
	    print " (defined)";
	} else {
	    print " (undefined)";
	}
	if (defined $item->{value}) {
	    print " value=\"$item->{value}\"";
	}
	print "\n";
    }
}


# -------------------------------------------------------------------------


sub parse_argv {
    my $ctxt = shift;
    my $adefs = shift;
    my $argv = shift;
    my $arg;
    my $optName;
    my $argId;
    my $argValue;
    my $errorCnt = 0;
    my $falseFl;
    my $defitem;
    my $defs;

    foreach $defitem (@$adefs) {
	$defs->{$defitem->{optName}} = $defitem;
    }

    foreach $arg (@$argv) {
	$argId = undef;
	$argValue = undef;
	$optName = undef;
	$falseFl = undef;
	$arg =~ s/#.*//;
	$arg =~ s/^\s+//;
	$arg =~ s/\s+$//;
#	print "Looking at arg \"$arg\"\n";
	if (($argValue) = $arg =~ /^\$(.+)$/) {
	    $arg = $ENV{$argValue};
	    if (!defined $arg) {
		my $line = "No such environment variable \"$argValue\"";
#		print STDERR "$line\n";
		$errorCnt++;
		next;
	    }
	    $errorCnt += parse_argv($ctxt,$defs,[$arg]);
	    next;
	}
	if (($argValue) = $arg =~ /^@(.+)$/) {
	    my $data = read_file($argValue);
	    $errorCnt += parse_argv($ctxt,$defs,$data);
	    next;
	}
	if (($falseFl,$optName) = $arg =~ /^--(no)?([^=]+)$/) {
	    # Nothing else to do
	} elsif (($optName,$argValue) = $arg =~ /^--(.+)=(.*)$/) {
	    # Nothing else to do
	} elsif (($optName,$argId,$argValue) = $arg =~ /^-(.)(.+)=(.*)$/) {
	    # Nothing else to do
	} elsif (($optName,$argId) = $arg =~ /^-(.)(.+)$/) {
	    # Nothing else to do
	} elsif (($optName,$argId) = $arg =~ /^-(.+)$/) {
	    # Nothing else to do
	} else {
	    $optName = "<arg>";
	    $argValue = $arg;
	}
	$optName =~ s/^\s+//;
	$optName =~ s/\s+$//;
#	print "optName is $optName\n";
	if (defined $argId) {
	    $argId =~ s/^\s+//;
	    $argId =~ s/\s+$//;
#	    print "argId is $argId\n";
	}
	if (defined $argValue) {
	    $argValue =~ s/^\s+//;
	    $argValue =~ s/\s+$//;
#	    print "argValue is $argValue\n";
	}
	if (! exists $defs->{$optName}) {
	    print STDERR "Unknown option name \"$optName\" in $arg\n";
	    $errorCnt++;
	    next;
	}
#	print "Found definition...\n";
	$defitem = $defs->{$optName};
	my $fl;
	my $foo = {
	    context => $ctxt,
	    def => $defitem,
	};
	$fl = 0;
	$fl = $defitem->{argId} if (defined $defitem->{argId});
	if ($fl == 0) {
	    if (defined $argId) {
		print STDERR "Unexpected argId \"$argId\" for $arg\n";
		$errorCnt++;
		next;
	    }
	} elsif ($fl == 1) {
	    if (! defined $argId) {
		print STDERR "Missing argId required for $arg\n";
		$errorCnt++;
		next;
	    }
	}
	$fl = 0;
	$fl = $defitem->{argValue} if (defined $defitem->{argValue});
	if ($fl == 0) {
	    if (defined $argValue) {
		print STDERR "Unexpected argValue \"$argValue\" for $arg\n";
		$errorCnt++;
		next;
	    }
	} elsif ($fl == 1) {
	    if (! defined $argValue) {
		print STDERR "Missing argValue required for $arg\n";
		$errorCnt++;
		next;
	    }
	}
	$fl = 0;
	$fl = $defitem->{falseFl} if (defined $defitem->{falseFl});
	if ($fl == 0) {
	    if (defined $falseFl) {
		print STDERR "Unexpected \"no\" prefix for $arg\n";
		$errorCnt++;
		next;
	    }
	}
	$foo->{argId} = $argId if (defined $argId);
	$foo->{argValue} = $argValue if (defined $argValue);
	$foo->{falseFl} = $falseFl if (defined $falseFl);
	&{$defitem->{setopt}}($foo);
	next;
    }
    return $errorCnt;
}


sub processFile {
    my $args = shift;
    my $fname = $args->{name};
    my $d1;
    print "Processing \"$fname\"\n";
    return if ($args->{dryRun});
    $d1 = read_file($fname);
    if ((exists $args->{symtab}) ||
	(exists $args->{inctab})) {
	$d1 = cstrip_data($args->{symtab},$args->{inctab},$d1);
    }
    $d1 = istrip_data($d1) if ($args->{istrip});
    write_file($fname,$d1);
}


# -------------------------------------------------------------------------


sub get_svn_file_list {
    my $args = shift;
    my $repositoryPath = $args->{srcDir};
    my $fh = new IO::File("svn list $repositoryPath |")
	or die "Unable to retrieve tag list\n";
    my @filelist;
    my $line;

    while (defined ($line = <$fh>)) {
	chomp $line;
	push @filelist,$line;
    }
    if (!scalar(@filelist)) {
	die "Unable to generate list of known snapshot tags\n";
    }
    return \@filelist;
}


# -------------------------------------------------------------------------


sub doProcessGroup {
    my $args = shift;
    my %hitList;
    my $name;
    my $dn;
    my $srcPath;
    my $dstPath;
    my $transferList = $args->{transferList};
    my $cp = "cp";
    my @dstNames;
    my $d1;

    $cp = $args->{cp} if (defined $args->{cp});

    print "Copying from $args->{srcDir} to $args->{dstDir} with \"$cp\"...\n";
    foreach $name (sort keys %$transferList) {
	next if (!defined $transferList->{$name});
	$srcPath = "$args->{srcDir}/$name";
	$dn = $transferList->{$name};
	$dstPath = "$args->{dstDir}/$dn";
	push @dstNames,$dn;
	optSystem($args->{dryRun},"$cp $srcPath $dstPath");
    }
    if ($args->{istrip} || exists $args->{symtab}) {
	if ($args->{istrip} && exists $args->{symtab}) {
	    print "ID and symbol stripping files...\n";
	} elsif ($args->{istrip}) {
	    print "ID stripping files...\n";
	} else {
	    print "Symbol stripping files...\n";
	}
	foreach $name (sort @dstNames) {
	    $dstPath = "$args->{dstDir}/$name";
	    print "  Processing $dstPath\n";
	    next if ($args->{dryRun});
	    $d1 = read_file($dstPath);
	    if ((exists $args->{symtab}) ||
		(exists $args->{inctab})) {
		$d1 = cstrip_data($args->{symtab},$args->{inctab},$d1);
	    }
	    if ($args->{istrip}) {
		$d1 = istrip_data($d1);
	    }
	    write_file($dstPath,$d1);
	}
    }
}


# -------------------------------------------------------------------------


sub setupCopy {
    my $ctxt = shift;
    my @fileList;
    my $name;
    my $transferList = $ctxt->{transferList};
    my $dirName = $ctxt->{srcDir};
    my $dirh;

    $dirh = new IO::Dir $dirName || die "Unable to examine $dirName\n";

    @fileList = sort $dirh->read();

    foreach $name (@fileList) {
	next if ($name =~ /^\..*$/);
	next if ($name =~ /^CVS$/);
	next if ($name =~ /^.*~$/);
	next if ($name =~ /^.*\.o$/);
	next if ($name =~ /^.*\.ko$/);
	next if ($name =~ /^.*\.o\.flags$/);
	next if ($name =~ /^.*\.mod\.c$/);
	next if (exists $transferList->{$name});
	$transferList->{$name} = $name;
    }

    $ctxt->{cp} = "cp";
}


sub setupSnap {
    my $args = shift;
    my $fileList;
    my $name;
    my $transferList = $args->{transferList};

    $args->{srcDir} = "$args->{repository}/$args->{srcTag}/driver";
    $args->{dstDir} = "$args->{dstSandbox}/$args->{dstTag}";

    $fileList = get_svn_file_list($args);

    foreach $name (@$fileList) {
	next if (exists $transferList->{$name});
	$transferList->{$name} = $name;
    }

    $args->{cp} = "svn copy";
};


# -------------------------------------------------------------------------


my $optSrcTag = {
    optName => "srcTag",
    argValue => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{srcTag} = $foo->{argValue};
    },
};


my $optSrcDir = {
    optName => "srcDir",
    argValue => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{srcDir} = $foo->{argValue};
    },
};


my $optDstDir = {
    optName => "dstDir",
    argValue => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{dstDir} = $foo->{argValue};
    },
};


my $optDstTag = {
    optName => "dstTag",
    argValue => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{dstTag} = $foo->{argValue};
    },
};


my $optDstSandbox = {
    optName => "dstSandbox",
    argValue => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{dstSandbox} = $foo->{argValue};
    },
};


my $optFile = {
    optName => "<arg>",
    argValue => 1,
    setopt => sub {
	my $foo = shift;
	push @{$foo->{context}->{files}},$foo->{argValue};
    },
};


my $optDryRun = {
    optName => "dryRun",
    falseFl => 1,
    setopt => sub {
	my $foo = shift;
	if ($foo->{falseFl}) {
	    $foo->{context}->{dryRun} = 0;
	} else {
	    $foo->{context}->{dryRun} = 1;
	}
    },
};


my $optTransfer = {
    optName => "T",
    argId => 1,
    argValue => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{transferList}->{$foo->{argId}} = $foo->{argValue};
    },
};


my $optIncName = {
    optName => "N",
    argId => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{inctab}->{$foo->{argId}} = undef;
    },
};


my $optDefine = {
    optName => "D",
    argId => 1,
    argValue => 2,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{symtab}->{$foo->{argId}} = {
	    defined => 1,
	    value => $foo->{argValue},
	};
    },
};


my $optUndefine = {
    optName => "U",
    argId => 1,
    setopt => sub {
	my $foo = shift;
	$foo->{context}->{symtab}->{$foo->{argId}} = {
	    defined => 0,
	};
    },
};



# -------------------------------------------------------------------------

my $snap_symtab = {
    PVR2_ENABLE_V4LCVS => {
	defined => 1,
    },
    PVR2_ENABLE_V4L2COMMON => {
	defined => 1,
    },
    PVR2_ENABLE_V4L2IOCTL => {
	defined => 1,
    },
    PVR2_ENABLE_OLDAUDC => {
	defined => 0,
    },
    PVR2_ENABLE_V4L2DEV => {
	defined => 1,
    },
    PVR2_ENABLE_NEW_ROUTING => {
	defined => 1,
    },
    PVR2_CX2341X_HDR_MEDIA => {
	defined => 1,
    },
    PVR2_ENABLE_CS53L32A => {
	defined => 1,
    },
    PVR2_ENABLE_OLD_SEMAPHORES => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_URBASYNC => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_OWNERSTUFF => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_I2CSTUFF => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_SYSFS_HOTPLUG => {
	defined => 0,
    },
    PVR2_ENABLE_V4L_PRINT_IOCTL => {
	defined => 1,
    },
    PVR2_ENABLE_TVEEPROM_FAKECLIENT => {
	defined => 1,
    },
    PVR2_ENABLE_TUNER_SETUPADDR => {
	defined => 1,
    },
    PVR2_ENABLE_SAA711X => {
	defined => 1,
    },
    PVR2_ENABLE_LOG_STATUS => {
	defined => 1,
    },
    PVR2_ENABLE_REGISTER_ACCESS => {
	defined => 1,
    },
    PVR2_ENABLE_OLDREGISTER_ACCESS => {
	defined => 0,
    },
    PVR2_ENABLE_OLD2REGISTER_ACCESS => {
	defined => 0,
    },
    PVR2_ENABLE_SAA7115 => {
	defined => undef,
    },
    PVR2_ENABLE_CX2584X_V4L => {
	defined => 1,
    },
    PVR2_ENABLE_WM8775 => {
	defined => 1,
    },
    PVR2_ENABLE_STDADJ => {
	defined => 0,
    },
    PVR2_ENABLE_STDEEPROMHACK => {
	defined => 1,
    },
    PVR2_ENABLE_OLD_MODULE_OWNER => {
	defined => 0,
    },
    PVR2_EEPROM_IVTV => {
	defined => undef,
    },
    V4L2_STD_NTSC_443 => {
	defined => 1,
    },
    V4L2_STD_NTSC_M_KR => {
	defined => 1,
    },
    V4L2_STD_SECAM_LC => {
	defined => 1,
    },
    PVR2_ENABLE_CX25840_FWSEND_HACK => {
	defined => undef,
    },
    PVR2_ENABLE_CX25840_ALGO_HACK => {
	defined => undef,
    },
    PVR2_ENABLE_CX2341XMOD => {
	defined => 1,
    },
    PVR2_ENABLE_CX2341XOLD => {
	defined => 0,
    },
    PVR2_ENABLE_CX2341XOLD2 => {
	defined => 0,
    },
    PVR2_ENABLE_CX2341XOLD3 => {
	defined => 0,
    },
    PVR2_ENABLE_DVB_NO_ADAPTER_NR => {
	defined => 0,
    },
    PVR2_ENABLE_EXT_CTRLS => {
	defined => 1,
    },
    PVR2_ENABLE_AUTOMODESWITCH => {
	defined => 0,
    },
    PVR2_ENABLE_TDA9887 => {
	defined => 0,
    },
    PVR2_EEPROM_INDIRECT => {
	defined => undef,
    },
    PVR2_ENABLE_I2CALGOCONTROL => {
	defined => 0,
    },
    PVR2_ENABLE_V4L1HARDWARE => {
	defined => 0,
    },
    PVR2_ENABLE_DEVICE_CREATOR => {
	defined => 1,
    },
    PVR2_ENABLE_DEVICE_AV400 => {
	defined => 0,
    },
    TUNER_LG_TDVS_H06XF => {
	defined => 1,
    },
    TUNER_PHILIPS_FCV1236D => {
	defined => 1,
    },
    PVR2_ENABLE_TDA18271_OUTPUT_LT_OFF => {
	defined => 1,
    },
    PVR2_ENABLE_V4L_TYPE => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_V4L_INODE1 => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_V4L_INODE2 => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_V4L_DEVNUM => {
	defined => 0,
    },
    PVR2_ENABLE_NO_DEV_NAME_API => {
	defined => 0,
    },
    PVR2_ENABLE_INITUSBRESET => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_V4L_FILEOPS => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_MODULE_INIT => {
	defined => 0,
    },
    PVR2_ENABLE_OLD_I2COPS => {
	defined => 0,
    },
    PVR2_ENABLE_V4L2SUBDEV => {
	defined => 1,
    },
    PVR2_ENABLE_V4L2SUBDEV_DISCONNECT => {
	defined => 1,
    },
    PVR2_ENABLE_V4L2SUBDEV_THRASH1 => {
	defined => 1,
    },
    PVR2_ENABLE_V4L2SUBDEV_THRASH2 => {
	defined => 1,
    },
    PVR2_ENABLE_NOSETTOGGLE => {
	defined => 1,
    },
    PVR2_ENABLE_OLD_DVB_TDA10048 => {
	defined => 0,
    },
    PVR2_ENABLE_DEV_PARENT => {
	defined => 1,
    },
    PVR2_ENABLE_DEV_PARENT_OLD => {
	defined => 0,
    },
};

my $snap_inctab = {
    'pvrusb2-options.h' => undef,
};

my $snap_transfer_changes = {
    'pvrusb2-options.h' => undef,
    'pvrusb2-video-ivtv.c' => undef,
    'pvrusb2-video-ivtv.h' => undef,
    'pvrusb2-demod.c' => undef,
    'pvrusb2-demod.h' => undef,
    'pvrusb2-i2c-cmd-v4l1.h' => undef,
    'pvrusb2-i2c-cmd-v4l1.c' => undef,
    'pvrusb2-i2c-cmd-v4l2.h' => undef,
    'pvrusb2-i2c-cmd-v4l2.c' => undef,
    'pvrusb2-i2c-track.h' => undef,
    'pvrusb2-i2c-track.c' => undef,
    'pvrusb2-i2c-chips-all.c' => undef,
    'pvrusb2-i2c-chips-v4l2.c' => undef,
    'pvrusb2-tuner.c' => undef,
    'pvrusb2-tuner.h' => undef,
    'pvrusb2-compat.c' => undef,
    'pvrusb2-cx2584x-old.c' => undef,
    'pvrusb2-cx2584x-old.h' => undef,
    'pvrusb2-audio-old.c' => undef,
    'pvrusb2-audio-old.h' => undef,
    'pvrusb2-version.h' => undef,
    'pvrusb2-sysfs-sel.c' => undef,
    'pvrusb2-sysfs-cd.c' => undef,
    'pvrusb2-dvb-wrap.c' => undef,
    'compat.h' => undef,
    'Modules.symvers' => undef,
    'Module.symvers' => undef,
    'Kbuild' => undef,
    'Makefile' => undef,
    'README' => undef,
};

sub doCopy {
    my $argv = shift;
    my $ctxt = {
	istrip => 1,
	symtab => $snap_symtab,
	inctab => $snap_inctab,
	transferList => $snap_transfer_changes,
    };
    my $defs = [
		$optDryRun,
		$optDefine,
		$optUndefine,
		$optTransfer,
		$optIncName,
		$optSrcDir,
		$optDstDir,
		];

    # Parse argv
    if (parse_argv($ctxt,$defs,$argv) > 0) {
	die "Command line parsing problem; aborting\n";
    }

    # Set up snapshot
    setupCopy($ctxt);

    # Put files in sandbox and strip accordingly
    doProcessGroup($ctxt);
}

sub doSnap {
    my $argv = shift;
    my $ctxt = {
	repository => "svn+ssh://cnc.isely.net/home/cvs/svn/pvrusb2",
	srcTag => "trunk",
	dstTag => "test_snap",
	dstSandbox => "pvrusb2_snap",
	istrip => 1,
	symtab => $snap_symtab,
	inctab => $snap_inctab,
	transferList => $snap_transfer_changes,
    };
    my $defs = [
		$optDryRun,
		$optDefine,
		$optUndefine,
		$optTransfer,
		$optIncName,
		$optSrcTag,
		$optDstTag,
		$optDstSandbox,
		];

    # Parse argv
    if (parse_argv($ctxt,$defs,$argv) > 0) {
	die "Command line parsing problem; aborting\n";
    }

    # Set up snapshot
    setupSnap($ctxt);

    # Create sandbox area
    optSystem($ctxt->{dryRun},
	      "svn checkout -N $ctxt->{repository}/tags $ctxt->{dstSandbox}");
    optSystem($ctxt->{dryRun},"svn mkdir $ctxt->{dstSandbox}/$ctxt->{dstTag}");

    # Put files in sandbox and strip accordingly
    doProcessGroup($ctxt);
}


# -------------------------------------------------------------------------


sub doIStrip {
    my $argv = shift;
    my $ctxt = {
	files => [ ],
	istrip => 1,
    };
    my $defs = [$optFile,$optDryRun];
    my $name;

    # Parse argv
    if (parse_argv($ctxt,$defs,$argv) > 0) {
	die "Command line parsing problem; aborting\n";
    }

    foreach $name (@{$ctxt->{files}}) {
	$ctxt->{name} = $name;
	processFile($ctxt);
    }
}


# -------------------------------------------------------------------------


sub doCStrip {
    my $argv = shift;
    my $ctxt = {
	files => [ ],
	symtab => { },
    };
    my $defs = [$optFile,$optDefine,$optUndefine,$optIncName,$optDryRun];
    my $name;

    # Parse argv
    if (parse_argv($ctxt,$defs,$argv) > 0) {
	die "Command line parsing problem; aborting\n";
    }

#    dump_symtab($ctxt->{symtab});

    foreach $name (@{$ctxt->{files}}) {
	$ctxt->{name} = $name;
	processFile($ctxt);
    }
}


# -------------------------------------------------------------------------


sub doit {
    my $helperCmds = {
	'snap' => \&doSnap,
	'copy' => \&doCopy,
	'idStrip' => \&doIStrip,
	'symStrip' => \&doCStrip,
    };

    die "Need command word.\n" if (scalar(@ARGV) < 1);

    my $cmd = $helperCmds->{$ARGV[0]};
    die "Unknown command $ARGV[0].\n" if (!defined $cmd);
    shift @ARGV;

    &$cmd(\@ARGV);
}

doit();

0;

