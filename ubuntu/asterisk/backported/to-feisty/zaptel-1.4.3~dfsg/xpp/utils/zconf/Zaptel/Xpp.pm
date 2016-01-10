package Zaptel::Xpp;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: Xpp.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;
use Zaptel::Xpp::Xbus;

my $proc_base = "/proc/xpp";

# Static Functions

# Nominal sorters for xbuses
sub by_name {
	return $a cmp $b;
}

sub by_connector {
	return $a->connector cmp $b->connector;
}

sub xbuses {
	my $optsort = shift || 'SORT_NAME';
	my @xbuses;

	-d "/proc/xpp" or return ();
	open(F, "$proc_base/xbuses") ||
		die "$0: Failed to open $proc_base/xbuses: $!\n";
	while(<F>) {
		chomp;
		my ($name, @attr) = split;
		$name =~ s/://;
		$name =~ /XBUS-(\d\d)/ or die "Bad XBUS number: $name";
		my $num = $1;
		@attr = map { split(/=/); } @attr;
		my $xbus = Zaptel::Xpp::Xbus->new(NAME => $name, NUM => $num, @attr);
		push(@xbuses, $xbus);
	}
	close F;
	my $sorter;
	if($optsort eq "SORT_CONNECTOR") {
		$sorter = \&by_connector;
	} elsif($optsort eq "SORT_NAME") {
		$sorter = \&by_name;
	} elsif(ref($optsort) eq 'CODE') {
		$sorter = $optsort;
	} else {
		die "Unknown optional sorter '$optsort'";
	}
	return sort $sorter @xbuses;
}

sub xpd_of_span($) {
	my $span = shift or die "Missing span parameter";
	return undef unless defined $span;
	foreach my $xbus (Zaptel::Xpp::xbuses('SORT_CONNECTOR')) {
		foreach my $xpd ($xbus->xpds()) {
			return $xpd if $xpd->fqn eq $span->name;
		}
	}
	return undef;
}

sub sync {
	my $newsync = shift;
	my $result;
	my $newapi = 0;

	my $file = "$proc_base/sync";
	die "$file is missing" unless -f $file;
	# First query
	open(F, "$file") or die "Failed to open $file for reading: $!";
	while(<F>) {
		chomp;
		/SYNC=/ and $newapi = 1;
		s/#.*//;
		if(/\S/) {	# First non-comment line
			s/^SYNC=\D*// if $newapi;
			$result = $_;
			last;
		}
	}
	close F;
	if(defined($newsync)) {		# Now change
		open(F, ">$file") or die "Failed to open $file for writing: $!";
		if($newsync eq 'HOST') {
			print F "HOST";
		} elsif($newsync =~ /^(\d+)$/) {
			print F ($newapi)? "SYNC=$1" : "$1 0";
		} else {
			die "Bad sync parameter '$newsync'";
		}
		close(F) or die "Failed in closing $file: $!";
	}
	return $result;
}

1;
