package Zaptel::Xpp::Xpd;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: Xpd.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;

my $proc_base = "/proc/xpp";

# Accessors (miniperl does not have Class:Accessor)
our $AUTOLOAD;
sub AUTOLOAD {
	my $self = shift;
	my $name = uc($AUTOLOAD);
	$name =~ s/.*://;   # strip fully-qualified portion
	if (@_) {
		return $self->{$name} = shift;
	} else {
		return $self->{$name};
	}
}

sub blink($$) {
	my $self = shift;
	my $on = shift;
	my $result;

	my $file = "$proc_base/" . $self->fqn . "/blink";
	die "$file is missing" unless -f $file;
	# First query
	open(F, "$file") or die "Failed to open $file for reading: $!";
	$result = <F>;
	chomp $result;
	close F;
	if(defined($on) and $on ne $result) {		# Now change
		open(F, ">$file") or die "Failed to open $file for writing: $!";
		print F ($on)?"1":"0";
		if(!close(F)) {
			if($! == 17) {	# EEXISTS
				# good
			} else {
				undef $result;
			}
		}
	}
	return $result;
}

sub zt_registration($$) {
	my $self = shift;
	my $on = shift;
	my $result;

	my $file = "$proc_base/" . $self->fqn . "/zt_registration";
	die "$file is missing" unless -f $file;
	# First query
	open(F, "$file") or die "Failed to open $file for reading: $!";
	$result = <F>;
	chomp $result;
	close F;
	if(defined($on) and $on ne $result) {		# Now change
		open(F, ">$file") or die "Failed to open $file for writing: $!";
		print F ($on)?"1":"0";
		if(!close(F)) {
			if($! == 17) {	# EEXISTS
				# good
			} else {
				undef $result;
			}
		}
	}
	return $result;
}

sub new($$) {
	my $pack = shift or die "Wasn't called as a class method\n";
	my $self = { @_ };
	bless $self, $pack;
	my $dir = "$proc_base/" . $self->fqn;
	$self->{DIR} = $dir;
	open(F, "$dir/summary") || die "Missing summary file in $dir";
	my $head = <F>;
	chomp $head;
	# "XPD-00 (BRI_TE ,card present, span registered) SYNC MASTER"
	close F;
	$head =~ s/^.*\(//;
	$head =~ s/\) */, /;
	$head =~ s/\s*,\s*/,/g;
	my ($type,$present,$registered,$sync) = split(/,/, $head);
	$self->{TYPE} = uc($type);
	$self->{IS_SYNC_MASTER} = ($sync =~ /MASTER/);
	return $self;
}

1;
