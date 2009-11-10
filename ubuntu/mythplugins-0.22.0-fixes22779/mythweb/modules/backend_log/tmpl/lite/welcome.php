<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/backend_log/tmpl/lite/welcome.php $
 * @date        $Date: 2009-08-02 06:50:00 +0200 (Sun, 02 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Open with a div and an image
    echo '<div id="info_backend_log">',
         '<p>',
         '<a href="', root_url, Modules::getModuleProperity('backend_log', 'path'), '">',
         '<img src="', skin_url, '/img/backend_log.png" class="module_icon" border="0" />',
         '<h2>', Modules::getModuleProperity('backend_log', 'name'), '</h2></a>',
         '</p>',

// Print a basic overview of what this module does
         t('welcome: backend_log'),

// Close the div
         "</div>\n";
