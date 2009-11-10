<?php
/**
 * Display/save MythWeb session settings
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/tv/set_screens.php $
 * @date        $Date: 2008-07-06 23:22:26 +0200 (Sun, 06 Jul 2008) $
 * @version     $Revision: 17728 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

    $Screens = array('upcoming recordings'  => array('title', 'original airdate', 'episode number', 'channel', 'recording group', 'airdate', 'record date', 'length'),
                    );

// Save?
    if ($_POST['save']) {
        foreach(array_keys($Screens) as $key)
            $_SESSION['settings']['screens']['tv'][$key] = $_POST[str_replace(' ', '_', $key)];
    }
