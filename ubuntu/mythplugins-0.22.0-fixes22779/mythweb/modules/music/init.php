<?php
/**
 * Initialization routines for the MythWeb Music module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/music/init.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Music
 *
/**/

// First, we should check to see that MythMusic is configured.
// Make sure to turn off fatal errors in case the table isn't there.
    $db->disable_fatal_errors();
    $has_music = $db->query_col('SELECT COUNT(*) FROM music_songs');
    $db->enable_fatal_errors();
    $db->error(false);

// If music is enabled, add it to the list.
    if ($has_music && tmpl == 'default') {
        $Modules['music'] = array('path'        => 'music',
                                  'sort'        => 2,
                                  'name'        => t('Music'),
                                  'description' => t('')
                                 );
    }

