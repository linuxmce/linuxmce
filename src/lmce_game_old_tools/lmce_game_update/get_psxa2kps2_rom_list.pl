#!/usr/bin/perl -w
#############################################################
##
## Grabs snapshots from psxa2z.com for PS2 games 
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

$agent->get("http://www.psxa2z.com/MasterPS2LIST.html");

my $stream = HTML::TokeParser->new(\$agent->{content});

while (my $token = $stream->get_tag("a"))
{
    my $url = $token->[1]{href} || "-";
    my $romName = $stream->get_trimmed_text("/a");
    $romName =~ s/\ \*$//; ## Remove silly " *" at end of some names in list.
    $romName =~ s/\//\_/;  ## Replace / with _ so that we don't get stupid path problems
    $_ = $url;
    my $urlMatch = /ps2/;

    if ($urlMatch)
    {
	push(@romList,$url."|".$romName);
	#print "$romName: $url\n";
    }
}

# we now have a complete list of ROMs to iterate through.

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
	my $urlMatch = /popImage/;
	if ($urlMatch)
	{
	    $picsURL =~ s/javascript\:popImage\(\'//;
	    $picsURL =~ s/\'\,\'\'\)//;
	    
            $_ = $picsURL;
	    my $special = /PS0585/;
	    if ($special)
            {
		# 0585 is missing, skip it.
		last;
            }

	    # We now have our image, the ps2 stuff is much easier to get.
	    $agent->get("$picsURL");
	    my $imgContent = $agent->{content};
	    open FILE, ">".$romName.".jpg" or die "writing romName $romName $!";
	    print "Writing JPEG file: $romName.jpg\n";
	    print FILE $imgContent;
	    close FILE;
	    last;
	}
    }
}
