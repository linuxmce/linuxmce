<?php
/**
 * This file is part of MythWeb, a php-based interface for MythTV.
 * See http://www.mythtv.org/ for details.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/includes/defines.php $
 * @date        $Date: 2009-10-09 08:08:47 +0200 (Fri, 09 Oct 2009) $
 * @version     $Revision: 22332 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Figure out the root path for this mythweb installation.  We need this in order
// to cleanly reference things like the /js directory from subpaths.
    if (isset($_SERVER['ORIG_SCRIPT_NAME']))
        define('root', str_replace('//', '/', dirname($_SERVER['ORIG_SCRIPT_NAME']).'/'));
    else
        define('root', str_replace('//', '/', dirname($_SERVER['SCRIPT_NAME']).'/'));

// Several sections of this program require the current hostname
    if (function_exists('posix_uname'))
        $uname = posix_uname();
    elseif (function_exists('php_uname'))
        $uname = php_uname('n');
    else
        throw new Exception('Failed to get server hostname!');

    define('hostname', empty($_SERVER['hostname']) ? trim($uname['nodename']) : $_SERVER['hostname']);
    unset($uname);

// Define the error email, or set it to a null string if there isn't a valid one
    define('error_email', array_key_exists('error_email', $_SERVER) && strstr($_SERVER['error_email'], '@')
                          ? $_SERVER['error_email']
                          : '');

/**
 * $Path is an array of PATH_INFO passed into the script via mod_rewrite or some
 * other lesser means.  It contains most of the information required for
 * figuring out what functions the user wants to access.
 *
 * @global  array   $GLOBALS['Path']
 * @name    $Path
/**/
    $Path = '';

    if (isset($_SERVER['ORIG_PATH_INFO']))
        $Path = $_SERVER['ORIG_PATH_INFO'];
    elseif (isset($_SERVER['PATH_INFO']))
        $Path = $_SERVER['PATH_INFO'];
    elseif (isset($_ENV['PATH_INFO']))
        $Path = $_ENV['PATH_INFO'];
    elseif (isset($_GET['PATH_INFO']))
        $Path = $_GET['PATH_INFO'];

// Convert extra whitespace
    $Path = preg_replace('/[\s]+/', ' ', $Path);

// Remove leading slashes
    $Path = preg_replace('/^\/+/', '', $Path);

    $Path = explode('/', $Path);

// Find the modules path
    $path = dirname(dirname(find_in_path('modules/tv/init.php')));
    if ($path == '')
       $path = dirname(find_in_path('modules/welcome.php'));
    define('modules_path', $path);

// Handy reference to the current module
    foreach ($Path as $path) {
        if (is_dir(modules_path.'/'.$path)) {
            define('module', $path);
            break;
        }
    }

// Set up some constants used by nice_filesystem()
    define('kb', 1024);         // Kilobyte
    define('mb', 1024 * kb);    // Megabyte
    define('gb', 1024 * mb);    // Gigabyte
    define('tb', 1024 * gb);    // Terabyte

// Define the http host used for access

    define('http_host', isset($_SERVER['HTTP_X_FORWARDED_HOST']) ? $_SERVER['HTTP_X_FORWARDED_HOST'] : $_SERVER['HTTP_HOST']);

    $host = http_host;
    $port = $_SERVER['SERVER_PORT'];

    if (strpos(http_host, ':') !== false)
        list($host, $port) = explode(':', http_host);

    $_SERVER['HTTP_HOST'] = $host;
    $_SERVER['HTTP_PORT'] = $port;

    if ($_SERVER['HTTP_PORT'] == '443' || $_SERVER['HTTP_PORT'] == '8443')
        $_SERVER['HTTPS'] = 'on';

    define('root_url',   ((isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on') ? 'https://' : 'http://' ).http_host.root);

    $stream_url = 'http';
    if (@$_SERVER['HTTPS'] == 'on' && !@$_SESSION['stream']['force_http'])
        $stream_url .='s';
    $stream_url .= '://'.$_SERVER['HTTP_HOST'];
    if (@$_SESSION['stream']['force_http'] && @$_SESSION['stream']['force_http_port'] > 0)
        $stream_url .= ":{$_SESSION['stream']['force_http_port']}";
    elseif ($_SERVER['HTTP_PORT'] > 0)
        $stream_url .= ":{$_SERVER['HTTP_PORT']}";
    $stream_url .= '/'.root;

    define('stream_url', $stream_url);
