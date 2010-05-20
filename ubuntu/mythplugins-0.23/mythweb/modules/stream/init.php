<?php
/**
 * Initialization routines for the MythWeb Streaming module
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/stream/init.php $
 * @date        $Date: 2007-12-31 12:38:38 -0800 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Music
 *
/**/

    $Settings['stream'] = array('name'    => t('Streaming'),
                                'choices' => array('protocol'  => t('Protocol'),
                                                  ),
                                'default' => 'protocol',
                               );

