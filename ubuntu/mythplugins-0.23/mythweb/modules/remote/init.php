<?php
/**
 * Initialization routines for the MythFrontend Remote Control
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/remote/init.php $
 * @date        $Date: 2008-11-30 19:40:35 -0800 (Sun, 30 Nov 2008) $
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
