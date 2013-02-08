#!/usr/bin/perl -w

# Perl trim function to remove whitespace from the start and end of the string
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

while (my $currentLine = <STDIN>)
{
	$currentLine =~ s/.zip//;
	if (index($currentLine,"(") != -1)
	{
		$currentLine = substr($currentLine,0,index($currentLine,"("));
	}
	else
	{
		$currentLine = substr($currentLine,0,index($currentLine,"#"));
	}
	$currentLine = trim($currentLine);
	print $currentLine."\n";
}
