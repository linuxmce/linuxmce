<?php
/**
 * The display code for the main welcome page that lists the available mythweb
 * sections.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/status/tmpl/wap/status.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Set the desired page title
    $page_title = 'MythTV Backend Status';

// Custom headers
    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'/status.css" />';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';

// Print the content itself
    echo "<div id=\"content_wrapper\">\n"
        .$status
        ."\n</div>";

// Print the page footer
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';

