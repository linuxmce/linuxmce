package Zaptel::Hardware::USB;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: USB.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;
use Zaptel::Hardware;
use Zaptel::Xpp;
use Zaptel::Xpp::Xbus;

my @idlist = qw(
		e4e4:1130
		e4e4:1131
		e4e4:1132
		e4e4:1140
		e4e4:1141
		e4e4:1142
		e4e4:1150
		e4e4:1151
		e4e4:1152
	);


$ENV{PATH} .= ":/usr/sbin:/sbin:/usr/bin:/bin";
my $prog = 'lsusb';

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

my @devices;
my @xbuses = Zaptel::Xpp::xbuses('SORT_CONNECTOR');

sub usb_sorter() {
	return
		sprintf("%03d/%03d", $a->bus, $a->dev) cmp
		sprintf("%03d/%03d", $b->bus, $b->dev);
}

sub xbus_of_usb($$) {
	my $bus = shift;
	my $dev = shift;

	my ($wanted) = grep {
		my $b = $_->usb_bus;
		my $d = $_->usb_dev;
		defined($b) && $b == $bus &&
		defined($d) && $d == $dev
	} @xbuses;
	return $wanted;
}

sub new($$) {
	my $pack = shift or die "Wasn't called as a class method\n";
	my $self = { @_ };
	bless $self, $pack;
	my $xbus = xbus_of_usb($self->bus, $self->dev);
	if(defined $xbus) {
		$self->{XBUS} = $xbus;
		$self->{DRIVER} = 'xpp_usb';
	}
	my $hardware_name = sprintf("usb:%03d/%03d", $self->{BUS}, $self->{DEV});
	$self->{HARDWARE_NAME} = $hardware_name;
	Zaptel::Hardware::device_detected($self, $hardware_name);
	return $self;
}

sub devices($) {
	my $pack = shift or die "Wasn't called as a class method\n";
	return sort usb_sorter @devices;
}

sub scan_devices($) {
	my $pack = shift || die;
	open(F, "$prog|") || die "$0: Failed running $prog: $!";
	while(<F>) {
		chomp;
		my ($bus,$dev,$id) = (split(/\s+/))[1,3,5];
		$dev =~ s/://;
		$bus =~ s/^0*//;
		$dev =~ s/^0*//;
		next unless grep { lc($id) eq $_ } @idlist;
		my($vendor,$product) = split(/:/, $id);
		my $d = Zaptel::Hardware::USB->new(
			BUS	=> $bus,
			DEV	=> $dev,
			VENDOR	=> $vendor,
			PRODUCT	=> $product,
			);
		push(@devices, $d);
	}
	close F;
}

1;
