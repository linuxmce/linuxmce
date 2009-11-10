<?php
/**
 * Initialization routines for the MythFrontend Remote Control
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/remote/init.php $
 * @date        $Date: 2008-12-01 04:40:35 +0100 (Mon, 01 Dec 2008) $
 * @version     $Revision: 19189 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Remote
 *
/**/

    if (tmpl == 'default' || tmpl == 'iPod') {
    // Add the module
        $Modules['remote'] = array('path'        => 'remote',
                                   'sort'        => 6,
                                   'name'        => t('Remote'),
                                   'links'       => array(#'jump' => t('Jump Points'),
                                                          'keys' => t('Key Controls'),
                                                          #'play' => t('Play'),
                                                         ),
                                  );
    }
