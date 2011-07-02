#!/usr/bin/perl -w
#############################################################
##
## Grabs snapshots from psxa2z.com for PS1 and PS2 games 
##
## Author: Thomas Cherryhomes <thom.cherryhomes@gmail.com>
##
#############################################################

use strict;
use WWW::Mechanize;
use URI::Escape;
use Switch;

my $agent = WWW::Mechanize->new();
my @romList = ();
my $i;

$agent->get("http://www.psxa2z.com/MasterPSXLIST4.html");

my $stream = HTML::TokeParser->new(\$agent->{content});

while (my $token = $stream->get_tag("a"))
{
    my $url = $token->[1]{href} || "-";
    my $romName = $stream->get_trimmed_text("/a");
    $romName =~ s/\ \*$//; ## Remove silly " *" at end of some names in list.
    $romName =~ s/\//\_/;  ## Replace / with _ so that we don't get stupid path problems
    $_ = $url;
    my $urlMatch = /gpgs/;

    if ($urlMatch)
    {
	push(@romList,$url."|".$romName);
	$i++;
    }
}

# we now have a complete list of ROMs to iterate through.
# At this point, we push through each one, grabbing its page

$i=0;

foreach (@romList)
{
    my @romParts = split(/\|/,$_);
    my $romURL = $romParts[0];
    my $romName = $romParts[1];
    my $picsURL;

    print "Grabbing ROM Detail Page for $romURL\n";

    $agent->get($romURL);
    $stream = HTML::TokeParser->new(\$agent->{content});
    while (my $token = $stream->get_tag("a"))
    {
	$picsURL = $token->[1]{href} || "-";
	$_ = $picsURL;
	my $urlMatch = /ss\//;
	if ($urlMatch)
	{
	    $agent->get($picsURL);
	    $stream = HTML::TokeParser->new(\$agent->{content});
	    while (my $token = $stream->get_tag("img"))
	    {
		my $imgURL = $token->[1]{src} || "-";
		$_ = $imgURL;
		my $imgURLMatch = /BT/;
		if ($imgURLMatch)
		{
		    # We found an image, dump the sucker to a file.
		    $agent->get("$imgURL");
		    my $imgContent = $agent->{content};
		    open FILE, ">".$romName.".jpg" or die "writing romName $romName $!";
		    print "Writing JPEG file: $romName.jpg\n";
		    print FILE $imgContent;
		    close FILE;
		}
	    }
	    last;
	}
    }

    

}

