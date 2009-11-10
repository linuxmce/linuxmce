<?php
/**
 * The display code for the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/status/tmpl/default/status.php $
 * @date        $Date: 2008-02-10 10:51:19 +0100 (Sun, 10 Feb 2008) $
 * @version     $Revision: 15887 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Status
 *
/**/

// Set the desired page title
    $page_title = 'MythTV Backend Status';

// Custom headers
    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'/status.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';

// Print the content itself
    echo "<div id=\"content_wrapper\">\n"
        .str_replace(array('<br />','<hr />'), array('<br>','<hr>'),$status)
        ."\n</div>";

// Print the page footer
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
