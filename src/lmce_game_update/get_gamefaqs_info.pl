#!/usr/bin/perl -w
#############################################################
## 
## Grabs information for a given ROM from GameFAQs 
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
my $picsURL;
my $boxURL;
my $picsDetailURL;
my $boxImgURL;
my $imgType;
my $boxImageLinkMatch;

while ($romName = <STDIN>)
{
	$boxURL = "";
	$picsURL = "";
	$picsDetailURL = "";
	$boxImgURL = "";
	$imgType=0;
	chomp($romName);
	## Get page, open stream to its contents in prep for scrape
	$agent->get("http://www.gamefaqs.com/search/index.html?platform=31&game=".uri_escape($romName)."&contrib=0&genre=0&region=0&date=0&developer=&publisher=&dist=0&sort=0&link=0&res=0&title=0");
	my $stream = HTML::TokeParser->new(\$agent->{content});
	
	while (my $token = $stream->get_tag("a"))
	{
		my $currentLink = $token->[1]{href};
		$_ = $currentLink;
		my $boxLinkMatch = /images/;
		if ($boxLinkMatch) # We want first match.
		{
			$picsURL = $_;
			last;
		}
	}

	if ($picsURL ne "")
	{
		$agent->get($picsURL);
		$stream = HTML::TokeParser->new(\$agent->{content});
		while (my $token = $stream->get_tag("a"))
		{
			my $currentLink = $token->[1]{href};
			$_ = $currentLink;
			if (/box-/) # First matching picture.
			{
				$boxURL = $_;
				last;
			}
		}
	}

	## Get box art page,
	if ($boxURL ne "")
	{
		print "Grabbing pic page for: ".$boxURL."\n\n";
		$agent->get($boxURL);
		$stream = HTML::TokeParser->new(\$agent->{content});
		
		while (my $token = $stream->get_tag("img"))
		{
			my $currentLink = $token->[1]{src};
			$_ = $currentLink;
			if (/_front/) # We want first match
			{
				$boxImgURL = $_;
				print "Getting IMG $boxImgURL\n";
				last;
			}
		}
		
		print "Grabbing Image ".$boxImgURL."\n";
		
		$agent->get("$boxImgURL");
		my $imgContent = $agent->{content};
		
		if ($imgType==3) # AtariAge Screenshots are in PNG format.
		{
			open FILE, ">".$romName.".png" or die $!;
			print FILE $imgContent;
			close FILE;
		}
		else
		{
			open FILE, ">".$romName.".jpg" or die $!;
			print FILE $imgContent;
			close FILE;
		}
	
	}
	else
	{
		print "I did not find any matching imagery for $romName !\n";
	}
}
