<?php
/**
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/database/set_settings.php $
 * @date        $Date: 2008-02-22 13:35:09 -0800 (Fri, 22 Feb 2008) $
 * @version     $Revision: 16202 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

    $tables = $db->query('SHOW TABLES');
    while ($table = $tables->fetch_col()) {
        if ($_REQUEST['action'] == t('Optimize Tables')) {
            $db->query("OPTIMIZE TABLE $table");
            $db->query("ANALYZE TABLE $table");
        }

        if ($_REQUEST['action'] == t('Repair Tables'))
            $db->query("REPAIR TABLE $table");

        if ($_REQUEST['action'] != t('Extended Check'))
            $Tables[$table]['check']  = $db->query_assoc("CHECK TABLE $table");
        else
            $Tables[$table]['check']  = $db->query_assoc("CHECK TABLE $table EXTENDED");
    }


