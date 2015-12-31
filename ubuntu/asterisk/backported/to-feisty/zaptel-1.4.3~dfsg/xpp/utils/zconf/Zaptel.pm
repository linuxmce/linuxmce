package Zaptel;
#
# Written by Oron Peled <oron@actcom.co.il>
# Copyright (C) 2007, Xorcom
# This program is free software; you can redistribute and/or
# modify it under the same terms as Perl itself.
#
# $Id: Zaptel.pm 2536 2007-05-17 21:47:40Z tzafrir $
#
use strict;
use Zaptel::Span;

my $proc_base = "/proc/zaptel";

sub spans() {
	my @spans;

	-d $proc_base or return ();
	foreach my $zfile (glob "$proc_base/*") {
		$zfile =~ s:$proc_base/::;
		my $span = Zaptel::Span->new($zfile);
		push(@spans, $span);
	}
	return sort { $a->num <=> $b->num } @spans;
}

1;
