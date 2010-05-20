<?php
/**
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/tv/list_recording_groups.php $
 * @date        $Date: 2009-08-08 23:05:45 -0700 (Sat, 08 Aug 2009) $
 * @version     $Revision: 21183 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
/**/

    $sh = $db->query('SELECT DISTINCT recorded.recgroup
                        FROM recorded');
    while ($group = $sh->fetch_col())
        $Groups[] = $group;

    $Groups[] = 'All';

    $Page_Previous_Location = root_url.'tv/submenu';
    $Page_Previous_Location_Name = 'Television';
    $Page_Title_Short = 'Rec Groups';

// Load the class for this page
    require_once tmpl_dir.'list_recording_groups.php';
