<?php
/**
 * This file is part of MythWeb, a php-based interface for MythTV.
 * See http://www.mythtv.org/ for details.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/includes/class_autoload.php $
 * @date        $Date: 2009-09-15 17:25:15 -0700 (Tue, 15 Sep 2009) $
 * @version     $Revision: 21877 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Sometimes a function will use if class_exists, and thus we shouldn't fail if we can't find it.

    function autoload($className) {
        global $Path;
        $className = str_replace('_', '/', $className);
        if (file_exists("classes/$className.php"))
            include_once "classes/$className.php";
        elseif (file_exists(modules_path.'/'.module."/classes/$className.php"))
            include_once modules_path.'/'.module."/classes/$className.php";
        elseif (file_exists(modules_path.'/'.$Path[1]."/classes/$className.php"))
            include_once modules_path.'/'.$Path[1]."/classes/$className.php";
        elseif (file_exists(modules_path.'/'.$Path[0]."/classes/$className.php"))
            include_once modules_path.'/'.$Path[0]."/classes/$className.php";
        else
            return false;
    }

    spl_autoload_register('autoload');
