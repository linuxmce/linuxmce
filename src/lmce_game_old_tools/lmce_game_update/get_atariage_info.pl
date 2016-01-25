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
my $boxImgURL;
my $imgType;
my $boxImageLinkMatch;

while ($romName = <STDIN>)
{
	$boxURL = "";
	$boxImgURL = "";
	$imgType=0;
	chomp($romName);
	## Get page, open stream to its contents in prep for scrape
	$agent->get("http://www.atariage.com/software_list.html?SystemID=7800&searchValue=".uri_escape($romName));
	print "Grabbing URL: http://www.atariage.com/software_list.html?SystemID=7800&searchValue=".uri_escape($romName)."\n\n";
	my $stream = HTML::TokeParser->new(\$agent->{content});
	
	while (my $token = $stream->get_tag("a"))
	{
		my $currentLink = $token->[1]{href};
		$_ = $currentLink;
		my $boxLinkMatch = /BoxStyleID/;
		if ($boxLinkMatch) # We want first match.
		{
			$imgType=1;
			$boxURL = $_;
			last;
		}
	}

	# if that fails, try grabbing a link to the cartridge bits.
	if ($boxURL eq "")
	{
		my $sStream = HTML::TokeParser->new(\$agent->{content});
		while (my $token = $sStream->get_tag("a"))
		{
			my $currentLink = $token->[1]{href};
			$_ = $currentLink;
			my $cartLinkMatch = /cart\_page/;
			if ($cartLinkMatch) # we want first match.
			{
				print "No Box art, but I found a cartridge scan!\n";
				$imgType=2;
				$boxURL= $_;
				last;
			}
		}
	}
	
	# if THAT fails, try grabbing a Screenshot.
	if ($boxURL eq "")
	{
                my $sStream = HTML::TokeParser->new(\$agent->{content});
                while (my $token = $sStream->get_tag("a"))
                {
                        my $currentLink = $token->[1]{href};
                        $_ = $currentLink;
                        my $cartLinkMatch = /screenshot/;
                        if ($cartLinkMatch) # we want first match.
                        {
                                print "No Box art or Cartridge scan, but I found a screenshot!\n";
                                $imgType=3;
                                $boxURL= $_;
                                last;
                        }
                }
	}

	## Get box art page,
	if ($boxURL ne "")
	{
		my $imgSType;
		switch ($imgType)
		{
			case 0
			{
				$imgSType = "NO";
			}
			case 1
			{
				$imgSType = "Box";
			}
			case 2
			{
				$imgSType = "Cartridge";
			}
			case 3
			{
				$imgSType = "Screenshot";
			}
		}

		print "Grabbing ".$imgSType." art for: ".$boxURL."\n\n";
		$agent->get($boxURL);
		$stream = HTML::TokeParser->new(\$agent->{content});
		
		while (my $token = $stream->get_tag("img"))
		{
			my $currentLink = $token->[1]{src};
			$_ = $currentLink;
			my $currMatch;
			switch ($imgType)
			{
				case 0 { }
				case 1 { $currMatch = /_front/ }
				case 2 { $currMatch = /_front/ }
				case 3 { $currMatch = /.png/ }
			}
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
		print "I did not find any matching imagery!\n";
	}
}
