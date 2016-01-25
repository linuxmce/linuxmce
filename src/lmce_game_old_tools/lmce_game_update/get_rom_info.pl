#!/usr/bin/perl -w
##############################################################
##
## Grabs information for a given ROM from MAWS.
##
## Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
##
##############################################################

use strict;

use WWW::Mechanize;
use HTML::TokeParser;

my $agent = WWW::Mechanize->new();
my $romName;

$romName = $ARGV[0];

## Get page, and open stream to its contents in preparation for scrape.
$agent->get("http://maws.mameworld.info/maws/romset/" . $romName);
my $stream = HTML::TokeParser->new(\$agent->{content});


## And scrape it...
$stream->get_tag("table");
$stream->get_tag("td");
$stream->get_tag("td");
 
my $title = $stream->get_trimmed_text("td");
$stream->get_tag("td");
$stream->get_tag("td");
my $setname = $stream->get_trimmed_text("td");
$stream->get_tag("td");
$stream->get_tag("td");
my $manufacturer = $stream->get_trimmed_text("td");
$stream->get_tag("td");
$stream->get_tag("td");
my $year = $stream->get_trimmed_text("td");
$stream->get_tag("td");
$stream->get_tag("td");
my $genre = $stream->get_trimmed_text("td");
$stream->get_tag("td");
$stream->get_tag("td");
my $category = $stream->get_trimmed_text("td");


## Output as a tokenizable string.
print $title . "|";
print $manufacturer . "|";
print $year . "|";
print $category;
