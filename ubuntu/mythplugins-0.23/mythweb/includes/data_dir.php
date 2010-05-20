<?php
/**
 * This file is part of MythWeb, a php-based interface for MythTV.
 * See http://www.mythtv.org/ for details.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/includes/data_dir.php $
 * @date        $Date: 2009-01-17 01:05:29 -0800 (Sat, 17 Jan 2009) $
 * @version     $Revision: 19707 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Make sure the data directory exists and is writable
    if (!is_dir('data') && !mkdir('data', 0755)) {
        custom_error('Error creating the data directory. Please check permissions.');
    }
    if (!is_writable('data')) {
        $process_user = posix_getpwuid(posix_geteuid());
        custom_error('data directory is not writable by '.$process_user['name'].'. Please check permissions.');
    }
