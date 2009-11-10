<?php
/**
 * This is pretty much a dummy module to hold the settings sections that
 * pertain to Mythbackend and other things not actually present within MythWeb
 * itself.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/mythtv/init.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
/**/

    $Settings['mythtv'] = array('name'    => t('MythTV'),
                                'choices' => array('settings' => t('Settings Table'),
                                                   'keys'     => t('Key Bindings'),
                                                  ),
                                'default' => 'settings',
                               );

