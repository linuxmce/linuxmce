<?php
/**
 * Display/save mythweb default settings
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/mythweb/set_defaults.php $
 * @date        $Date: 2009-07-31 20:07:05 -0700 (Fri, 31 Jul 2009) $
 * @version     $Revision: 21085 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// Save?
    if ($_POST['save']) {
    // Some global mythweb settings
        $_SESSION['prefer_channum']      = setting('WebPrefer_Channum', null, $_POST['prefer_channum'] ? 1 : 0);
        $_SESSION['show_popup_info']     = $_POST['show_popup_info']     ? 1 : 0;
        $_SESSION['show_channel_icons']  = $_POST['show_channel_icons']  ? 1 : 0;
        $_SESSION['sortby_channum']      = $_POST['sortby_channum']      ? 1 : 0;
        $_SESSION['genre_colors'] = $_POST['genre_colors'] ? 1 : 0;
        $_SESSION['show_video_covers']   = $_POST['show_video_covers']   ? 1 : 0;
    }

// These settings are limited to MythWeb itself
    $Settings_Hosts = 'MythWeb';
