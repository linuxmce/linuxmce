#!/usr/bin/perl
###############################################################
##
## A new scraper based on grabbing data from -listxml
##
## Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
##
###############################################################

use strict;
use XML::Parser;

my $mamedata = `mame @ARGV -listxml`;

# Tag States
my $g_btGame = 0;
my $g_btDescription = 0;
my $g_btYear = 0;
my $g_btManufacturer = 0;

# Parsed Data
my $g_sDescription = "";
my $g_sYear = "";
my $g_sManufacturer = "";
my $g_sGenre = "";
my $g_sCloneOf = "";

my $parser = new XML::Parser(Handlers => {
    Start => \&handler_start,
    End => \&handler_end,
    Char => \&handler_char,
    Default => \&handler_default});

$parser->parse($mamedata);

if ($g_sCloneOf eq "")
{
    $g_sGenre = get_genre(@ARGV);
}
else
{
    $g_sGenre = get_genre($g_sCloneOf);
}

print "$g_sDescription|$g_sManufacturer|$g_sYear|$g_sGenre";

################################################################################

sub handler_start
{
    my ($p, $elt, %atts) = @_;
    if ($elt eq "game")
    {
	$g_btGame = 1;
	# Special case to set cloneof if it exists.
	if (%atts->{'cloneof'} ne "")
	{
	    $g_sCloneOf = %atts->{'cloneof'};
	}
    }
    if ($elt eq "description")
    {
	$g_btDescription = 1;
    }
    if ($elt eq "manufacturer")
    {
	$g_btManufacturer = 1;
    }
    if ($elt eq "year")
    {
	$g_btYear = 1;
    }
}

sub handler_end
{
    my ($p, $elt) = @_;
    if ($elt eq "game")
    {
	$g_btGame = 0;
    }
    if ($elt eq "description")
    {
	$g_btDescription = 0;
    }
    if ($elt eq "manufacturer")
    {
	$g_btManufacturer = 0;
    }
    if ($elt eq "year")
    {
	$g_btYear = 0;
    }
}

sub handler_char
{
    my ($p, $str) = @_;
    if ($g_btGame == 1)
    {
	if ($g_btDescription == 1)
	{
	    $g_sDescription = $str;
	}
	elsif ($g_btManufacturer == 1)
	{
	    $g_sManufacturer = $str;
	}
	elsif ($g_btYear == 1)
	{
	    $g_sYear = $str;
	}
    }
}

sub handler_default
{
    ## Throw away everything else. 
}

sub get_genre
{
    my ($game) = @_;
    my $sGenre = "";
    open (FILE,"Category.ini");
    while (<FILE>)
    {
	chomp;
	if (/^\[/) # Bracket found. reset category.
	{
	    $sGenre = /\[(.+)\]/;
	    $sGenre = $1;
	}
	if (/$game/) # Game found. Return Genre.
	{
	    return $sGenre;
	}
    }
}
