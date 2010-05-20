<?php
/**
 * Welcome page description of the status module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/stats/tmpl/lite/welcome.php $
 * @date        $Date: 2009-08-01 21:50:00 -0700 (Sat, 01 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Stats
 *
/**/

// Open with a div and an image
    echo '<div id="info_stats">',
         '<p>',
         '<a href="', Modules::getModuleProperity('stats', 'path'), '">',
         '<img src="', skin_url, '/img/stats.png" class="module_icon" border="0" />',
         '<h2>', Modules::getModuleProperity('stats', 'name'), '</h2></a>',
         '</p>',

// Print a basic overview of what this module does
         t('welcome: stats'),

// Close the div
         "</div>\n";
