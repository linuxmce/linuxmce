<?php
/**
 * Initialization routines for the MythWeb Backend Logs module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/backend_log/init.php $
 * @date        $Date: 2009-01-20 07:42:03 +0100 (Tue, 20 Jan 2009) $
 * @version     $Revision: 19752 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Logs
 *
/**/

    global $db;

// Check to see if database logging has been enabled
    $enabled = $db->query_col('SELECT SUM(data) FROM settings WHERE value="LogEnabled"');

// The TV module is always enabled.
    if ($enabled > 0)
        $Modules['backend_log'] = array('path'        => 'backend_log',
                                        'sort'        => 550,
                                        'name'        => t('Logs'),
                                       );
