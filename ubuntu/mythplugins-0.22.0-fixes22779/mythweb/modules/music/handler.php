<?php
/**
 * MythMusic browser
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/music/handler.php $
 * @date        $Date: 2009-09-12 05:01:54 +0200 (Sat, 12 Sep 2009) $
 * @version     $Revision: 21787 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Music
 *
/**/

// Raw passthrough?
    if (substr($Path[1], -3) == '.js') {
        readfile('modules/music/'.$Path[1]);
        exit;
    }

// Load the modules we'll need
    if (in_array($Path[1], array('mp3act_js.js.php', 'mp3act_hidden.php'))) {
        require_once 'modules/music/'.$Path[1];
        exit;
    }

// Streaming handler will exit on its own
    if ($Path[1] == 'stream') {
        require_once 'modules/music/stream.php';
    }

// Too many messy headers/cookies errors in the main script -- wrapper it
// to avoid them.
    ob_start();

// Load the main module
    require_once 'modules/music/mp3act_main.php';

/**
 * Print the music streaming base URL.  Defined here because it's used by a
 * couple of different files called above.
/**/
    function stream_url() {
        return root_url.'music/';
    }
