package Zaptel::Span;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: Span.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;
use Zaptel::Chans;

my $proc_base = "/proc/zaptel";

sub chans($) {
	my $span = shift;
	return @{$span->{CHANS}};
}

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

sub by_number($) {
	my $span_number = shift;
	die "Missing span number" unless defined $span_number;
	my @spans = Zaptel::spans();

	my ($span) = grep { $_->num == $span_number } @spans;
	return $span;
}

my @bri_strings = (
		'BRI_(NT|TE)',
		'(?:quad|octo)BRI PCI ISDN Card.* \[(NT|TE)\]\ ',
		'octoBRI \[(NT|TE)\] ',
		'HFC-S PCI A ISDN.* \[(NT|TE)\] '
		);

our $ZAPBRI_NET = 'bri_net';
our $ZAPBRI_CPE = 'bri_cpe';

sub new($$) {
	my $pack = shift or die "Wasn't called as a class method\n";
	my $num = shift or die "Missing a span number parameter\n";
	my $self = { NUM => $num };
	bless $self, $pack;
	open(F, "$proc_base/$num") or die "Failed to open '$proc_base/$num\n";
	my $head = <F>;
	chomp $head;
	foreach my $cardtype (@bri_strings) {
		if($head =~ m/$cardtype/) {
			$self->{IS_BRI} = 1;
			$self->{TERMTYPE} = $1;
			$self->{DCHAN_IDX} = 2;
			$self->{BCHAN_LIST} = [ 0, 1 ];
			last;
		}
	}
	die "$0: Unkown TERMTYPE [NT/TE]\n"
		if $self->is_bri and !defined $self->{TERMTYPE};
	($self->{NAME}, $self->{DESCRIPTION}) = (split(/\s+/, $head, 4))[2, 3];
	$self->{IS_ZAPTEL_SYNC_MASTER} =
		($self->{DESCRIPTION} =~ /\(MASTER\)/) ? 1 : 0;
	$self->{CHANS} = [];
	while(<F>) {
		chomp;
		s/^\s*//;
		s/\s*$//;
		next unless /\S/;
		my ($chan, $name, $signalling, $info) = split(/\s+/, $_, 4);
		my $c = Zaptel::Chans->new($self, $chan, $name, $signalling, $info);
		push(@{$self->{CHANS}}, $c);
	}
	close F;
	if($self->is_bri()) {
		$self->{CODING} = 'ami';
		$self->{YELLOW} = undef;
		$self->{DCHAN} = ($self->chans())[$self->{DCHAN_IDX}];
		$self->{BCHANS} = [ ($self->chans())[@{$self->{BCHAN_LIST}}] ];
		# Infer some info from channel name:
		my $first_chan = ($self->chans())[0] || die "$0: No channels in span #$num\n";
		my $chan_fqn = $first_chan->fqn();
		if($chan_fqn =~ m(ZTHFC.*/|ztqoz.*/|XPP_BRI_.*/)) {		# BRI
			$self->{FRAMING} = 'ccs';
			$self->{SWITCHTYPE} = 'euroisdn';
			$self->{SIGNALLING} = ($self->{TERMTYPE} eq 'NT') ? $ZAPBRI_NET : $ZAPBRI_CPE ;
		} elsif($chan_fqn =~ m(ztgsm.*/)) {				# Junghanns's GSM cards. 
			$self->{FRAMING} = 'ccs';
			$self->{SIGNALLING} = 'gsm';
		}
	}
	return $self;
}

sub bchans($) {
	my $self = shift || die;

	return @{$self->{BCHANS}};
}

1;
