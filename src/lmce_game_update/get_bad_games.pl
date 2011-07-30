#!/usr/bin/perl
##############################################################
##
## List ROMs that are not playable.
##
## Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
##
##############################################################

use strict;
use XML::Parser;

my $g_btGame = 0;
my $g_btDriver = 0;

# attribute states.
my $g_bIsMechanical = 0;
my $g_bIsPreliminary = 0;
my $name;

my $parser = new XML::Parser(Handlers => {
    Start => \&handler_start,
    End => \&handler_end,
    Char => \&handler_char,
    Default => \&handler_default});

open(PS,"mame -listxml | ") || die("Could not Launch MAME.");

$parser->parse(*PS);

close(PS);

sub handler_start
{
    my $out = "";
    my ($p, $elt, %atts) = @_;
    if ($elt eq "game")
    {
	$g_btGame = 1;
	if (%atts->{'name'} ne "")
	{
		$name = %atts->{'name'};
	}
	if (%atts->{'ismechanical'} eq "yes")
	{
		$out .= $name . ".zip\n";
	}
    }
    if ($elt eq "driver")
    {
	if (%atts->{'status'} eq 'preliminary')
	{
		#print "Driver is preliminary $name\n";
	   	$out .= $name . ".zip\n";
	}
    }
	
	print $out;
}

sub handler_end
{
    my ($p, $elt) = @_;
    if ($elt eq "driver")
    {
	$g_btDriver = 0;
    }
    if ($elt eq "game")
    {	
	$g_btGame = 0;
	$name = "";
    }
}

sub handler_char
{
    ## No Character data to pull.
}

sub handler_default
{
    ## Don't need anything else.
}
