<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/status/tmpl/default/welcome.php $
 * @date        $Date: 2008-02-09 18:36:08 -0800 (Sat, 09 Feb 2008) $
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
