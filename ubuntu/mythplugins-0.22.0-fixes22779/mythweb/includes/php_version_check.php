<?php
/**
 * This file is part of MythWeb, a php-based interface for MythTV.
 * See http://www.mythtv.org/ for details.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/includes/php_version_check.php $
 * @date        $Date: 2008-06-24 01:32:57 +0200 (Tue, 24 Jun 2008) $
 * @version     $Revision: 17604 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/
    define('PHP_MIN_VERSION', 5.1);

// Make sure we're running a new enough version of php
    if (substr(phpversion(), 0, 3) < PHP_MIN_VERSION)
        trigger_error('You must be running at least php '.PHP_MIN_VERSION.' to use this program.', FATAL);
