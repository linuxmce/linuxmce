<?php
/**
 * Welcome page that lists the available mythweb sections
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/welcome.php $
 * @date        $Date: 2009-01-20 07:42:03 +0100 (Tue, 20 Jan 2009) $
 * @version     $Revision: 19752 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Default requested view
    if (empty($_REQUEST['view_module']) || !Modules::getModule($_REQUEST['view_module']))
        $_REQUEST['view_module'] = 'tv';

// Not really much to do here but print the list of modules
    require 'modules/_shared/tmpl/'.tmpl.'/welcome.php';
