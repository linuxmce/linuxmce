<?php
/**
 * An index for handy SQL searches in the listings data
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/tv/searches.php $
 * @date        $Date: 2009-07-31 21:00:02 -0700 (Fri, 31 Jul 2009) $
 * @version     $Revision: 21090 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
 *
/**/

// Load the canned searches
    require_once 'modules/tv/canned_searches.conf.php';
// Load the local canned searches (if it exists)
    if (file_exists('configuration/canned_searches.conf.php'))
        include 'configuration/canned_searches.conf.php';

// Load the class for this page
    require_once tmpl_dir.'searches.php';

// Exit
    exit;
