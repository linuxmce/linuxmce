<?php
/**
 * Welcome page description of the Remote Control module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/remote/tmpl/default/welcome.php $
 * @date        $Date: 2009-08-01 21:50:00 -0700 (Sat, 01 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Remote
 *
/**/

// Open with a div and an image
    echo '<div id="info_remote" class="hidden">',
         '<img src="', skin_url, 'img/tv.png" class="module_icon" alt="">',

// Print a basic overview of what this module does
         t('welcome: remote'),

// Next, print a list of possible frontends
         '<ul>';
    foreach (Modules::getModuleProperity('remote', 'links') as $link => $name) {
        echo '    <li><a href="', Modules::getModuleProperity('remote', 'path'), '/', $link, '">', html_entities($name), "</a></li>\n";
    }
    echo '</ul>',

// Close the div
         "</div>\n";
