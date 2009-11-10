<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/status/tmpl/lite/welcome.php $
 * @date        $Date: 2009-08-02 06:50:00 +0200 (Sun, 02 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Open with a div and an image
    echo '<div id="info_status">',
         '<p>',
         '<a href="', root_url, Modules::getModuleProperity('status', 'path'), '">',
         '<img src="', skin_url, '/img/status.png" class="module_icon" border="0" />',
         '<h2>', Modules::getModuleProperity('status', 'name'), '</h2></a>',
         '</p>',

// Print a basic overview of what this module does
         t('welcome: status'),

// Close the div
         "</div>\n";
