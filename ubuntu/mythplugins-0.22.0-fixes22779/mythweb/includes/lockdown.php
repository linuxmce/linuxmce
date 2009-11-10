<?php
/**
 * To attempt to curve the massive amounts of (unintentionally) open mythweb installs, we are attempting to
 * protect the users by having a automatic lockdown if we detect a 'bot' or if it's specifically requested by a user
 *
 * You can disable this feature with the apache env var of MYTHWEB_LOCKDOWN_DISABLE being set to true
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/includes/lockdown.php $
 * @date        $Date: 2008-06-24 01:32:57 +0200 (Tue, 24 Jun 2008) $
 * @version     $Revision: 17604 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

    if ($_SERVER['MYTHWEB_LOCKDOWN_DISABLE'] != true) {
        if (   stristr('bot',     $_SERVER['HTTP_USER_AGENT']) !== false
            || stristr('spider',  $_SERVER['HTTP_USER_AGENT']) !== false
            || stristr('crawler', $_SERVER['HTTP_USER_AGENT']) !== false
            || stristr('search',  $_SERVER['HTTP_USER_AGENT']) !== false
            || stristr('yahoo',   $_SERVER['HTTP_USER_AGENT']) !== false
            || isset($_GET['TRIGGER_MYTHWEB_LOCKDOWN'])
           )
            touch('lockdown');
    }

    if (   $_SERVER['MYTHWEB_LOCKDOWN_DISABLE'] != true && file_exists('lockdown'))
        tailored_error('lockdown');
