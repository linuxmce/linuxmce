<?php
/**
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/remote/play_program_on_frontend.php $
 * @date        $Date: 2009-08-02 06:50:00 +0200 (Sun, 02 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

    $chanid    = $_REQUEST['chanid'];
    $starttime = $_REQUEST['starttime'];

    if ($_REQUEST['host']) {
        $host = $_REQUEST['host'];
        $frontends = MythFrontend::findFrontends();
        $frontends[$host]->play_program($chanid, $starttime);
        header('Location: tv/detail/'.urlencode($chanid).'/'.urlencode($starttime));
        exit;
    }

    $Page_Previous_Location = 'tv/detail/'.urlencode($chanid).'/'.urlencode($starttime);
    $Page_Previous_Location_Name = 'Details';
    $Page_Title_Short = 'Pick Frontend';

// Display the page
    require_once tmpl_dir.'play_program_on_frontend.php';
