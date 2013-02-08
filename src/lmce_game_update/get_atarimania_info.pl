#!/usr/bin/perl -w
#############################################################
##
## Grabs information for a given ROM from Atarimania
##
## Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
##
#############################################################

use strict;

use WWW::Mechanize;
use HTML::TokeParser;
use URI::Escape;
use Switch;

my $agent = WWW::Mechanize->new();
my $romName;

while ($romName = <STDIN>)
{
    # Clear State
    chomp($romName);
    $agent->get("http://");
}
