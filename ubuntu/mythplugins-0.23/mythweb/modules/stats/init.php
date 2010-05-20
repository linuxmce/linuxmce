<?php
/**
 * Initialization routines for the MythWeb Stats module
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/stats/init.php $
 * @date        $Date: 2007-12-31 12:38:38 -0800 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Stats
 *
/**/

    $enabled = $db->query_col('SELECT COUNT(*) FROM oldrecorded');

    if($enabled) {
        $Modules['stats'] = array('path' => 'stats',
                                  'sort' => 501,
                                  'name' => t('Statistics'),
                                 );
    }
