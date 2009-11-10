<?php
/**
 * Initialization routines for the MythWeb Status module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/status/init.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Status
 *
/**/

// The TV module is always enabled.
    $Modules['status'] = array('path'        => 'status',
                               'sort'        => 500,
                               'name'        => t('Status'),
                          );

