package Zaptel::Xpp::Xbus;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: Xbus.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;
use Zaptel::Xpp::Xpd;

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

sub xpds($) {
	my $xbus = shift;
	return @{$xbus->{XPDS}};
}

sub by_number($) {
	my $busnumber = shift;
	die "Missing xbus number parameter" unless defined $busnumber;
	my @xbuses = Zaptel::Xpp::xbuses();

	my ($xbus) = grep { $_->num == $busnumber } @xbuses;
	return $xbus;
}

sub get_xpd_by_number($$) {
	my $xbus = shift;
	my $xpdnum = shift;
	die "Missing XPD number parameter" unless defined $xpdnum;
	my @xpds = $xbus->xpds;
	return $xpds[$xpdnum];
}

sub new($$) {
	my $pack = shift or die "Wasn't called as a class method\n";
	my $self = {};
	while(@_) {
		my ($k, $v) = @_;
		shift; shift;
		# Keys in all caps
		$k = uc($k);
		# Some values are in all caps as well
		if($k =~ /^(STATUS)$/) {
			$v = uc($v);
		}
		$self->{$k} = $v;
	}
	bless $self, $pack;
	$self->{NAME} or die "Missing xbus name";
	my $prefix = "$proc_base/" . $self->{NAME};
	my $usbfile = "$prefix/xpp_usb";
	if(open(F, "$usbfile")) {
		my $head = <F>;
		chomp $head;
		close F;
		$head =~ s/^device: +([^, ]+)/$1/i or die;
		my ($usb_bus, $usb_dev) = split('/', $head);
		die unless defined($usb_bus) && defined($usb_dev);
		$usb_bus =~ s/^0*//;
		$usb_dev =~ s/^0*//;
		$self->{USB_BUS} = $usb_bus;
		$self->{USB_DEV} = $usb_dev;
	}
	@{$self->{XPDS}} = ();
	foreach my $fqn (glob "$prefix/XPD-??") {
		$fqn =~ s:$proc_base/::;
		$fqn =~ /(\d+)$/;
		my $num = $1;
		my $xpd = Zaptel::Xpp::Xpd->new(
					FQN => $fqn,
					NUM =>, $num,
					XBUS => $self
					);
		push(@{$self->{XPDS}}, $xpd);
	}
	@{$self->{XPDS}} = sort { $a->num <=> $b->num } @{$self->{XPDS}};
	return $self;
}

1;
