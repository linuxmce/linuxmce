<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/status/tmpl/default/welcome.php $
 * @date        $Date: 2008-02-10 03:36:08 +0100 (Sun, 10 Feb 2008) $
 * @version     $Revision: 15879 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Open with a div and an image
    echo '<div id="info_status" class="hidden">',
         '<img src="', skin_url, '/img/status.png" class="module_icon" alt="">',

// Print a basic overview of what this module does
         t('welcome: status'),

// Close the div
         "</div>\n";
