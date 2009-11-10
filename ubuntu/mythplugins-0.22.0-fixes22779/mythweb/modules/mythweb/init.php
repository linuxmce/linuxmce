<?php
/**
 * This is pretty much a dummy module to hold the settings sections that
 * pertain specifically to MythWeb.  It allows us to have a prettier URL when
 * editing, which integrating these into the settings module wouldn't.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/mythweb/init.php $
 * @date        $Date: 2009-01-20 07:42:03 +0100 (Tue, 20 Jan 2009) $
 * @version     $Revision: 19752 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
/**/

    $Settings['mythweb'] = array('name'    => t('MythWeb'),
                                 'choices' => array('session'   => t('My Session'),
                                                    'defaults'  => t('MythWeb Defaults'),
                                                    'flvplayer' => t('Video Playback'),
                                                   ),
                                 'default' => 'session',
                                );
