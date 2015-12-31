package Zaptel::Hardware::PCI;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: PCI.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;
use Zaptel::Hardware;

my @idlist = qw(
	1397:16B8
	1397:08B4
	1057:5608
	10B5:3001
	10B5:4000
	10B5:9030
	10B5:D00D
	D161:0800
	D161:2400
	E159:0001
	);

$ENV{PATH} .= ":/usr/sbin:/sbin:/usr/bin:/bin";
my $prog = 'lspci';

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

sub pci_sorter() {
	return
		sprintf("%03d/%03d", $a->bus, $a->dev) cmp
		sprintf("%03d/%03d", $b->bus, $b->dev);
}

sub new($$) {
	my $pack = shift or die "Wasn't called as a class method\n";
	my $self = { @_ };
	bless $self, $pack;
	my $hardware_name = sprintf("pci:%s:%s:%s", $self->{DOMAIN}, $self->{BUS}, $self->{DEV});
	$self->{HARDWARE_NAME} = $hardware_name;
	Zaptel::Hardware::device_detected($self, $hardware_name);
	my $sysfile = sprintf "/sys/bus/pci/devices/%s:%s:%s/driver/module", $self->{DOMAIN}, $self->{BUS}, $self->{DEV};
	my $module = readlink($sysfile);
	if(defined $module) {
		$module =~ s:^.*/::;
		$self->{DRIVER} = $module;
	}
	return $self;
}

sub devices($) {
	my $pack = shift or die "Wasn't called as a class method\n";
	return sort pci_sorter @devices;
}

my $domain_support = 1;		# Optimistic...

sub scan_devices($) {
	my $pack = shift || die;
	if(!open(F, "$prog -Dn 2> /dev/null |")) {
		$domain_support = 0;
		open(F, "$prog -n|") || die "$0: Failed running $prog: $!";
	}
	while(<F>) {
		chomp;
		my ($phys,$id) = (split(/\s+/))[0,2];
		my $domain;
		my $bus;
		my $dev;
		if($domain_support) {
			($domain,$bus,$dev) = split(/:/, $phys);
		} else {
			($bus,$dev) = split(/:/, $phys);
			$domain = '0000';
		}
		next unless grep { uc($id) eq $_ } @idlist;
		my($vendor,$product) = split(/:/, $id);
		my $d = Zaptel::Hardware::PCI->new(
			DOMAIN	=> $domain,
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
