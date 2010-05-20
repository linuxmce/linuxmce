<?php
/**
 * Welcome page description of the Weather module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/weather/tmpl/default/welcome.php $
 * @date        $Date: 2008-02-10 01:51:19 -0800 (Sun, 10 Feb 2008) $
 * @version     $Revision: 15887 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/

// Open with a div and an image
    echo '<div id="info_weather" class="hidden">',
         '<img src="', skin_url, '/img/weather.png" class="module_icon">',

// Print a basic overview of what this module does
         t('welcome: weather'),

// Next, print a list of possible subsectons
    ####

// Close the div
         "</div>\n";
