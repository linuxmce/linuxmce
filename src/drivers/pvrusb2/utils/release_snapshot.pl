#!/usr/bin/perl

# $Id: release_snapshot.pl 1153 2006-05-17 04:39:09Z isely $

#
#  Copyright (C) 2005 Mike Isely <isely@pobox.com>
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

# This script does various things to help with snapshot creation.  It
# can create "full" snapshots or code intended to find its way into
# v4l (which has some slight changes).  This script automates
# creation of a version header which is included by pvrusb2-main.c
# into the driver compilation (no more screwed up version labels).
# This script can also automatically generate the HTML listing all
# known snapshots - it does this by querying the repository.

use strict;
use Getopt::Long;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use IO::File;
use IO::Dir;
use Text::Wrap;


# -------------------------------------------------------------------------

my $repository = "svn+ssh://cnc.isely.net/home/cvs/svn/pvrusb2";
my $source_tag = "trunk";
my $versionHeader = "driver/pvrusb2-version.h";
my $downloadHTMLFile = "download.html";
my $historyHTMLFile = "history.html";
my $historySrcFile = "change_history.txt";
my $driverSource = "www.isely.net";
my $snapshotName = undef;

# -------------------------------------------------------------------------

sub generate_version_header_name {
    my $fname = shift;
    my $id = shift;
    my $line;
    if (! defined $snapshotName) {
	die "Can't generate version info: Snapshot name is undefined\n";
    }
    my $fh = new IO::File("> $fname") or die "Unable to create $fname\n";
    print $fh "#ifndef _INCLUDED_PVRUSB2_DRIVER_VERSION\n";
    print $fh "#define _INCLUDED_PVRUSB2_DRIVER_VERSION\n";
    $line = $snapshotName;
    if (defined $id) {
	$line .= " (from $id)";
    }
    print $fh "#define DRIVER_VERSION \"$line\"\n";
    print $fh "#endif\n";
}

sub generate_version_header {
    if (!defined $versionHeader) {
	die "Can't generate version info: No version header file defined\n";
    }
    print "Generating version header file \"$versionHeader\"\n";
    generate_version_header_name($versionHeader,$driverSource);
}

sub get_driver_file_list {
    my $fh = new IO::File("svn list $repository/$source_tag/driver |")
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

sub get_snapshot_list {
    my $fh = new IO::File("svn list $repository/tags |")
	or return undef;
    my @taglist;
    my $line;
    my $tag;

    while (defined ($line = <$fh>)) {
	chomp $line;
	next if (!(($tag) = $line =~ /^mci-rel-(\d{8}\w?)\/$/));
	push @taglist,$tag;
    }
    if (!scalar(@taglist)) {
	die "Unable to generate list of known snapshot tags\n";
    }
    @taglist = sort @taglist;
    return \@taglist;
}

sub generate_download_page_data {
    my $fh = shift;
    my $taglist = shift;
    my $tagCount = scalar(@$taglist);
    my $idx;
    my $href;
    my $sid;

    print $fh <<EOF;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<META HTTP-EQUIV="CONTENT-TYPE" CONTENT="text/html; charset=iso-8859-1">
<TITLE>pvrusb2 Linux Driver Downloads</TITLE>
</HEAD>
<BODY LANG="en-US" DIR="LTR">
<HR>
EOF

    for ($idx = 0; $idx < $tagCount; $idx++) {
	$sid = $taglist->[$tagCount-($idx+1)];
	$href = "<A HREF=\"http://www.isely.net/downloads/pvrusb2-mci-$sid.tar.bz2\">pvrusb2-mci-$sid.tar.bz2</A> (<A HREF=\"history.html#pvrusb2-mci-$sid\">Change History</A>)";
	if ($idx == 0) {
	    print $fh "<P>Latest pvrusb2 driver snapshot:</P>\n";
	    print $fh "<blockquote>\n";
	    print $fh "<P>$href</P>\n";
	    print $fh "</blockquote>\n";
	    next;
	}
	if ($idx == 1) {
	    print $fh "<P>Older snapshots (version format is yyyymmdd):</P>\n";
	    print $fh "<blockquote>\n"
	}
	print $fh "$href<BR>\n";
	if (($idx+1) == $tagCount) {
	    print $fh "</blockquote>\n"
	}
    }
    print $fh <<EOF;
<HR>
</BODY>
</HTML>
EOF
}

sub generate_download_page_file {
    if (!defined $downloadHTMLFile) {
	die "Unable to generate download page: No file defined\n";
    }
    print "Generating HTML download file \"$downloadHTMLFile\"\n";
    my $taglist = get_snapshot_list();
    die "Unable to retrieve tag list\n" if (!defined $taglist);
    my $fh = new IO::File(">$downloadHTMLFile")
	or die "Unable to create $downloadHTMLFile\n";
    generate_download_page_data($fh,$taglist);
}

sub slurp_history_file {
    my $fname = shift;
    my $fh = new IO::File("<$fname")
	or die "Unable to read $fname\n";
    my @info;
    my $crec;
    my $line;
    my $idata;
    my $iflag = 0;
    my $id;
    my ($v1,$v2);

    while (defined ($line = <$fh>)) {
	chomp $line;
	$line =~ s/\s+$//;
	$line =~ s/^\s+//;
	if (length $line == 0) {
	    $iflag = 0;
	    next;
	}
	if (!$iflag) {
	    if (($v1,$v2) = ($line =~ /^\((.+?)\)\s+(.*)$/)) {
		# New item
		next if (!defined $crec);
		$idata = [ ];
		push @{$crec->{items}},{id => $v1, info => $idata};
		push @$idata,$v2;
	    } elsif (($id) = $line =~ /^\[(.+)\]$/) {
		$crec = {
		    items => [ ],
		    info => [ ],
		};
		push @info,$crec;
		$crec->{id} = $id;
		$idata = $crec->{info};
	    } else {
		# New paragraph within item or a summary paragraph
		next if (! defined $idata);
		push @$idata,$line;
	    }
	    $iflag = 1;
	    next;
	}
	$idata->[scalar(@$idata)-1] .= " $line";
    }

    return \@info;
}

sub htmlify_text {
    my $str = shift;
    $str =~ s/\&/\&amp;/g;
    $str =~ s/\</\&lt;/g;
    $str =~ s/\>/\&gt;/g;
    $str =~ s/"/\&quot;/g;
    return wrap('','',$str);
}

sub generate_history_page_data {
    my $fh = shift;
    my $dataTable = shift;
    my $taglist = shift;
    my $dataCount = scalar(@$dataTable);
    my ($idx1,$idx2);
    my ($cnt1);
    my $sid;
    my $item;
    my $str;
    my $cnt;
    my %downloadtags;

    $Text::Wrap::columns = 75;

    if (defined $taglist) {
	foreach $sid (@$taglist) {
	    $downloadtags{"pvrusb2-mci-$sid"} = 1;
	}
    }

    $cnt = 0;
    for ($idx1 = 0; $idx1 < $dataCount; $idx1++) {
	$cnt += scalar(@{$dataTable->[$idx1]->{items}});
    }

    print $fh <<EOF;
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<META HTTP-EQUIV="CONTENT-TYPE" CONTENT="text/html; charset=iso-8859-1">
<TITLE>pvrusb2 Linux Driver Change History</TITLE>
</HEAD>
<BODY LANG="en-US" DIR="LTR">
<HR>
<H1>pvrusb2 Linux Driver Change History</H1>
EOF

    print $fh "<P>There are $cnt changes across $dataCount snapshots shown here.</P>\n\n";

    for ($idx1 = 0; $idx1 < $dataCount; $idx1++) {
	$sid = $dataTable->[$idx1];
	print $fh "<P><A NAME=\"$sid->{id}\">$sid->{id}</A>";
	if (exists $downloadtags{$sid->{id}}) {
	    print $fh " <A HREF=\"http://www.isely.net/downloads/$sid->{id}.tar.bz2\">(download)</A>";
	}
	print $fh "</P>\n\n";
	$cnt1 = scalar(@{$sid->{info}});
	if (scalar($sid->{info})) {
	    for ($idx2 = 0; $idx2 < $cnt1; $idx2++) {
		print $fh "<BLOCKQUOTE>\n";
		print $fh htmlify_text($sid->{info}->[$idx2]), "\n";
		print $fh "</BLOCKQUOTE>\n\n";
	    }
	}

	next if (!scalar(@{$sid->{items}}));

	print $fh "<UL>\n\n";

	foreach $item (@{$sid->{items}}) {
	    $cnt1 = scalar(@{$item->{info}});
	    print $fh "<LI>\n";
	    if ($cnt1 == 1) {
		print $fh htmlify_text($item->{info}->[0]),"\n";
	    } else {
		for ($idx2 = 0; $idx2 < $cnt1; $idx2++) {
		    $str = htmlify_text("<P>" . $item->{info}->[$idx2] . "</P>");
		    print $fh $str,"\n";
		}
	    }
	    print $fh "</LI>\n\n";
	}

	print $fh "</UL>\n\n";
    }
    print $fh <<EOF;
<HR>
</BODY>
</HTML>
EOF
}

sub generate_history_page_file {
    if (!defined $historySrcFile) {
	die "Unable to generate history page: No input file defined\n";
    }
    if (!defined $historyHTMLFile) {
	die "Unable to generate history page: No output file defined\n";
    }
    print "Generating HTML history file \"$historyHTMLFile\"\n";
    print "Using source file \"$historySrcFile\"\n";
    my $taglist = get_snapshot_list();
    my $dataTable = slurp_history_file($historySrcFile);
    my $fh = new IO::File(">$historyHTMLFile")
	or die "Unable to create $historyHTMLFile\n";
    generate_history_page_data($fh,$dataTable,$taglist);
}

sub generate_sa_snapshot {
    if (! defined $snapshotName) {
	die "Can't generate snapshot: Snapshot name is undefined\n";
    }
    my $sandbox = "pvrusb2-tmp";
    my $tag1name = "mci-rel-$snapshotName";
    my $tag2name = "pvrusb2-mci-$snapshotName";
    system "svn checkout $repository/$source_tag $sandbox";
    generate_version_header_name("$sandbox/driver/pvrusb2-version.h",
				 $driverSource);
    system "svn add $sandbox/driver/pvrusb2-version.h";
    system "svn copy -m 'Release snapshot $tag1name' $sandbox $repository/tags/$tag1name";
    system "rm -rf $sandbox";
    system "svn export $repository/tags/$tag1name $tag2name";
    system "tar jcvf $tag2name.tar.bz2 $tag2name";
    system "rm -rf $tag2name";
}

sub generate_v4l_snapshot {
    if (! defined $snapshotName) {
	die "Can't generate snapshot: Snapshot name is undefined\n";
    }
    my %transferlist = (
	'pvrusb2-video-ivtv.c' => undef,
	'pvrusb2-video-ivtv.h' => undef,
	'pvrusb2-i2c-cmd-v4l1.h' => undef,
	'pvrusb2-i2c-cmd-v4l1.c' => undef,
	'pvrusb2-i2c-chips-all.c' => undef,
	'compat.h' => undef,
	'Kbuild' => undef,
	'Kbuild.v4l' => 'Kbuild',
	'Makefile' => undef,
	'Makefile.v4l' => 'Makefile',
    );
    my ($name1,$name2);
    my $sandbox = "pvrusb2-tmp";
    my $tag1name = "v4l-rel-$snapshotName";
    my $tag2name = "pvrusb2-v4l-$snapshotName";
    my $filelist = get_driver_file_list();
    system "svn mkdir -m '' $repository/tags/$tag1name";
    system "svn checkout $repository/tags/$tag1name $sandbox";
    foreach $name1 (@$filelist) {
	$name2 = $name1;
	if (exists $transferlist{$name1}) {
	    next if (! defined $transferlist{$name1});
	    $name2 = $transferlist{$name1};
	}
	system "svn copy $repository/$source_tag/driver/$name1 $sandbox/$name2";
    }
    generate_version_header_name("$sandbox/pvrusb2-version.h","v4l tree");
    system "svn add $sandbox/pvrusb2-version.h";
    system "svn commit -m 'Release snapshot $tag1name' $sandbox";
    system "rm -rf $sandbox";
    system "svn export $repository/tags/$tag1name $tag2name";
    system "tar jcvf $tag2name.tar.bz2 $tag2name";
    system "rm -rf $tag2name";
}

sub mainFunc {
    my $genVersionHeader;
    my $genDownloadFile;
    my $genHistoryFile;
    my $genSnapshot;
    my $genV4LSnapshot;
    my $didSomething = 0;
    exit 1 if (!GetOptions("sourceId=s" => \$driverSource,
			   "repository=s" => \$repository,
			   "source_tag=s" => \$source_tag,
			   "versionHeader=s" => \$versionHeader,
			   "downloadHTMLFile=s" => \$downloadHTMLFile,
			   "historyHTMLFile=s" => \$historyHTMLFile,
			   "historySourceFile=s" => \$historySrcFile,
			   "snapshot=s" => \$snapshotName,
			   "genSnapshot!" => \$genSnapshot,
			   "genV4LSnapshot!" => \$genV4LSnapshot,
			   "genVersionHeader!" => \$genVersionHeader,
			   "genDownloadFile!" => \$genDownloadFile,
			   "genHistoryFile!" => \$genHistoryFile,
			   ));
    if ($genDownloadFile) {
	generate_download_page_file();
	$didSomething = 1;
    }
    if ($genHistoryFile) {
	generate_history_page_file();
	$didSomething = 1;
    }
    if ($genVersionHeader) {
	generate_version_header();
	$didSomething = 1;
    }
    if ($genSnapshot) {
	generate_sa_snapshot();
	$didSomething = 1;
    }
    if ($genV4LSnapshot) {
	generate_v4l_snapshot();
	$didSomething = 1;
    }
    if (!$didSomething) {
	print "You didn't tell me something to do...\n";
    }
}

# -------------------------------------------------------------------------

# Don't autoflush stdout so that progress printing works OK.
autoflush STDOUT !0;

mainFunc();
