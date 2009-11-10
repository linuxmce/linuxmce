<?php
/**
 * Initialization routines for the MythWeb settings module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/settings/init.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/

// The settings module is always enabled.
    $Modules['settings'] = array('path'        => 'settings',
                                 'sort'        => 999,
                                 'name'        => t('Settings'),
                                 'description' => t('settings'),
                                );
