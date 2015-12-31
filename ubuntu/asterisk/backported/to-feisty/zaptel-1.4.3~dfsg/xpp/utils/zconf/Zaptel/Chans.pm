package Zaptel::Chans;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: Chans.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;

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

sub new($$$$$$) {
	my $pack = shift or die "Wasn't called as a class method\n";
	my $span = shift or die "Missing a span parameter\n";
	my $num = shift or die "Missing a channel number parameter\n";
	my $fqn = shift or die "Missing a channel fqn parameter\n";
	my $signalling = shift || '';
	my $info = shift || '';
	my $self = {};
	bless $self, $pack;
	$self->span($span);
	$self->num($num);
	$self->fqn($fqn);
	$self->signalling($signalling);
	$self->info($info);
	my $type;
	if($fqn =~ m|\bXPP_(\w+)/.*$|) {
		$type = $1;		# One of our AB
	} elsif(defined $signalling) {
		$type = 'FXS' if $signalling =~ /^FXS/;
		$type = 'FXO' if $signalling =~ /^FXO/;
	} else {
		$type = undef;
	}
	$self->type($type);
	return $self;
}

my $ztcfg = $ENV{ZTCFG} || '/sbin/ztcfg';
sub probe_type($) {
	my $self = shift;
	my $fqn = $self->fqn;
	my $num = $self->num;
	my $type;

	if($fqn =~ m:WCTDM/|\ WRTDM/|OPVXA1200/:) {
		my %maybe;

		undef %maybe;
		foreach my $sig (qw(fxo fxs)) {
			my $cmd = "echo ${sig}ks=$num | $ztcfg -c /dev/fd/0";

			$maybe{$sig} = system("$cmd >/dev/null 2>&1") == 0;
		}
		if($maybe{fxo} and $maybe{fxs}) {
			$type = 'EMPTY';
		} elsif($maybe{fxo}) {
			$type = 'FXS';
		} elsif($maybe{fxs}) {
			$type = 'FXO';
		}
	} else {
		$type = $self->type;
	}
	return $type;
}

1;
