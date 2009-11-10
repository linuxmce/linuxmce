<?php
/**
 * Display/save mythweb session settings
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/stream/set_protocol.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Music
 *
/**/

// Save?
    if ($_POST['save']) {
    // Save the protocol choice
        $_SESSION['stream']['force_http']       = $_REQUEST['force_http'] ? true : false;
        $_SESSION['stream']['force_http_port']  = intVal($_REQUEST['force_http_port']);
        if ($_SESSION['stream']['force_http_port'] < 1)
            $_SESSION['stream']['force_http_port'] = '';
    }
