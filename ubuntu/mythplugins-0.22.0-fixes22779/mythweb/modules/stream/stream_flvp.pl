#!/usr/bin/perl
#
# MythWeb Streaming/Download module
#
# @url       $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/stream/stream_flvp.pl $
# @date      $Date: 2009-01-23 06:45:53 +0100 (Fri, 23 Jan 2009) $
# @version   $Revision: 19791 $
# @author    $Author: xris $
#

# URI back to this file?  We just need to take the current URI and strip
# off the .flvp suffix.
    my $uri = ($ENV{'HTTPS'} || $ENV{'SERVER_PORT'} == 443)
                ? 'https'
                : 'http';
    $uri .= '://'.($ENV{'SERVER_NAME'} or $ENV{'SERVER_ADDR'}).':'.$ENV{'SERVER_PORT'}
            .$ENV{'REQUEST_URI'};
    $uri =~ s/\.flvp$/\.flv/i;
# Print a page to hold the player
    print header();
    print <<EOF;
<html>
<head>
</head>
<body>
<embed src="${web_root}tv/flvplayer.swf" width="320" height="260" bgcolor="#FFFFFF"
type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer"
flashvars="file=$uri&autoStart=true" />
</body>
EOF

    1;
