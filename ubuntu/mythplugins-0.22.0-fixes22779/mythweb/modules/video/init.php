<?php
/**
 * Initialization routines for the MythWeb Video module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/video/init.php $
 * @date        $Date: 2008-06-08 22:15:53 +0200 (Sun, 08 Jun 2008) $
 * @version     $Revision: 17452 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Video
 *
/**/

// First, we should check to see that MythVideo is configured.
// Make sure to turn off fatal errors in case the table isn't there.
    $db->disable_fatal_errors();
    $has_video = $db->query_col('SHOW TABLES LIKE "videometadata"');
    $db->enable_fatal_errors();
    $db->error(false);

// If video is enabled, add it to the list.
    if ($has_video && tmpl == 'default') {
        $Modules['video']  = array('path'        => 'video',
                                   'sort'        => 3,
                                   'name'        => t('Video'),
                                   'description' => t('')
                                  );
        $Settings['video'] = array('name'    => t('Video'),
                                   'choices' => array('settings' => t('Settings'),
                                                     ),
                                   'default' => 'settings',
                                  );

    }
