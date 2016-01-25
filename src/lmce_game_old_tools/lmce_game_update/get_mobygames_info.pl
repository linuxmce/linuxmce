#!/usr/bin/perl -w
#############################################################
## 
## Grabs information for a given ROM from AtariAge
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
my $boxURL;
my $detailsURL;
my $boxImgURL;
my $imgType;
my $boxImageLinkMatch;

while ($romName = <STDIN>)
{
	$boxURL = "";
	$boxImgURL = "";
	$detailsURL = "";
	$imgType=0;
	chomp($romName);
	## Get page, open stream to its contents in prep for scrape
	$agent->get("http://www.mobygames.com/search/quick?q=".uri_escape($romName)."&p=39&search=Go");
	my $stream = HTML::TokeParser->new(\$agent->{content});
	
	while (my $token = $stream->get_tag("a"))
	{
		my $currentLink = $token->[1]{href};
		$_ = $currentLink;
		my $boxLinkMatch = /\/game\//;
		if ($boxLinkMatch) # We want first match.
		{
			$imgType=1;
			$boxURL = $_;
			last;
		}
	}

	## Get details page
	if ($boxURL ne "")
	{
		print "Grabbing details page for $boxURL\n\n";
		$agent->get($boxURL);
		$stream = HTML::TokeParser->new(\$agent->{content});

		while (my $token = $stream->get_tag("a"))
		{
			my $currentLink = $token->[1]{href};
			$_ = $currentLink;
			my $currMatch = /gameCoverId/;
			if ($currMatch)
			{
				$detailsURL = $_;
				print "Getting Details Page $detailsURL\n";
			}
		}
	}

	## Get box art page,
	if ($detailsURL ne "")
	{
		my $imgSType = "Box";
		print "Grabbing ".$imgSType." art page for: ".$detailsURL."\n\n";
		$agent->get($detailsURL);
		$stream = HTML::TokeParser->new(\$agent->{content});
		
		while (my $token = $stream->get_tag("img"))
		{
			my $currentLink = $token->[1]{src};
			$_ = $currentLink;
			my $currMatch = /large/;
			if ($currMatch) # We want first match
			{
				$boxImgURL = $_;
				print "Getting IMG $boxImgURL\n";
				last;
			}
		}
		
		print "Grabbing Image ".$boxImgURL."\n";
		
		$agent->get("$boxImgURL");
		my $imgContent = $agent->{content};
		
		open FILE, ">".$romName.".jpg" or die $!;
		print FILE $imgContent;
		close FILE;
	
	}
	else
	{
		print "I did not find any matching imagery!\n";
	}
}
