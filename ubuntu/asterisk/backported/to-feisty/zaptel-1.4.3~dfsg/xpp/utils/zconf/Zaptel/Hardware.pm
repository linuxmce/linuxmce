package Zaptel::Hardware;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: Hardware.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;
use Zaptel::Hardware::USB;
use Zaptel::Hardware::PCI;

my @zaptel_devices;

sub device_detected($$) {
	my $dev = shift || die;
	my $name =  shift || die;
	warn "Device '$name' already known\n"
		if grep { $_->hardware_name eq $name } @zaptel_devices;
	push(@zaptel_devices, $dev);
}

sub device_removed($) {
	my $dev = shift || die;
	my $name = $dev->hardware_name;
	die "Missing zaptel device hardware name" unless $name;
	@zaptel_devices = grep { $_->hardware_name ne $name } @zaptel_devices;
}

sub devices($) {
	my $pack = shift || die;

	return @zaptel_devices;
}

Zaptel::Hardware::USB->scan_devices;
Zaptel::Hardware::PCI->scan_devices;

1;
