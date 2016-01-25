#!/usr/bin/perl -w
##############################################################
##
## Grabs (U) released PS1 games
##
##############################################################

use strict;

use WWW::Mechanize;
use HTML::TokeParser;
use URI::Escape;
use Switch;

use Regexp::Common qw /URI/;

sub trim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}
# Left trim function to remove leading whitespace
sub ltrim($)
{
    my $string = shift;
    $string =~ s/^\s+//;
    return $string;
}
# Right trim function to remove trailing whitespace
sub rtrim($)
{
    my $string = shift;
    $string =~ s/\s+$//;
    return $string;
}

my $agent = WWW::Mechanize->new();

print "Grabbing URL for PS1 Games... Please wait.\n";
# Grab the List of games page.
#$agent->get("http://www.emuparadise.me/Sony_Playstation_ISOs/List-All-Titles/2");
$agent->get("http://www.emuparadise.me/Sony_Playstation_ISOs/Games-Starting-With-Z/2");
my $stream = HTML::TokeParser->new(\$agent->{content});

while (my $token = $stream->get_tag("a"))
{
    my $currentLink  =$token->[1]{href};
    $_ = $currentLink;
    my $currentLinkMatch = /Sony_Playstation_ISOs/;
    if ($currentLinkMatch)
    {
	my $currentLink = $_;
	my $matchedLink = "";
	my $matchNonLink = /\/2$/;
	my $matchUS1 = /\[U\]/;
	my $matchUS2 = /\(U\)/;
	my $matchUS3 = /\[NTSC-U\]/;
	my $matchUS4 = /\(NTSC-U\)/;
	my $matchUS5 = /\(bin\)/;
	my $matchUS6 = /\(ccd\)/;
	my $matchUS7 = /\(mdf\)/;
	my $matchNoParens = /^((?!\(*\)).)*$/;
	if ($matchNonLink)
	{
	    #print "Metadata link found: $currentLink\n";
	}
	elsif ($matchUS1)
	{
	    $matchedLink = $_;
	} elsif ($matchUS2)
	{
	    $matchedLink = $_;
	} elsif ($matchUS3)
	{
	    $matchedLink = $_;
	} elsif ($matchUS4)
	{
	    $matchedLink = $_;
        } elsif ($matchUS5)
        {
            $matchedLink = $_;
        } elsif ($matchUS6)
        {
            $matchedLink = $_;
        } elsif ($matchUS7)
        {
            $matchedLink = $_;
	} elsif ($matchNoParens)
	{
	    $matchedLink = $_;
	    print "* Matched a non paren link: $_\n";
	} else
	{
	    # print "Non US Rom $currentLink\n";
	}
	if ($matchedLink ne "")
	{
	    print "\n----------------------------------------------------------------\n";
	    print "Matched Link: $matchedLink\n";
	    # Something was matched. Let's open the page and look for the megaupload mirror
	    my $agent2 = WWW::Mechanize->new();
	    $agent2->get("http://www.emuparadise.me/" . $matchedLink);
	    my $stream2 = HTML::TokeParser->new(\$agent2->{content});
	    my $token2;

	    # Yes, this is odd, but it should get what we need.

	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");
	    $token2 = $stream2->get_tag("h2");

	    my $matchU1 = $stream2->get_trimmed_text("h2");

	    # Special Case 1
	    if ($matchU1 =~ /Support Emu/)
	    {
		$token2 = $stream2->get_tag("h2");
		$matchU1 = $stream2->get_trimmed_text("h2");
	    }

	    $matchU1 =~ /$RE{URI}{-keep}/;
	   
	    if ($matchU1 =~ /megaupload/)
	    {
		# Megaupload mirror, let's grab it.
		$matchU1 =~ s/MegaUpload\ Mirror\:\ //;
		system("/usr/local/bin/megaupload-dl ".$matchU1);
	    }
	    else
	    {
		print "NOTMATCH\n";
	    }

	    # Get the image if available
	    my $stream3 = HTML::TokeParser->new(\$agent2->{content});

	    while (my $token3 = $stream3->get_tag("img"))
	    {
		my $currentImg = $token3->[1]{src};
		$_ = $currentImg;
		my $imgMatch = /front.jpg/;
		if ($imgMatch)
		{
		    my $agent4 = WWW::Mechanize->new();
		    $agent4->get($currentImg);
		    my $imgContent = $agent4->{content};
		    $currentImg =~ s/http\:\/\/www.emuparadise.org\/PSX\/Covers\///;
		    $currentImg =~ s/-front.jpg//;
		    $currentImg = substr($currentImg, 0, index($currentImg, "("));
		    $currentImg = substr($currentImg, 0, index($currentImg, "["));
		    $currentImg = trim($currentImg);
		    $currentImg = $currentImg . ".jpg";
		    print "IMAGE MATCH: $currentImg\n";
		    open FILE, ">./snap/".$currentImg or die $!;
		    print FILE $imgContent;
		    close FILE;
		}
		else
		{
		    # print "NOIMAGE MATCH: $currentImg\n";
		}
	    }

	}
    }

}
