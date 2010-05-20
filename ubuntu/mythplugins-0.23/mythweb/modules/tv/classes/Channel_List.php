<?php
/**
 * Class to hold a list of channels, along with
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/tv/classes/Channel_List.php $
 * @date        $Date: 2007-12-31 12:38:38 -0800 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
 *
/**/

class Channel_List {

/** protected @var array   Channel objects for each channel */
    var $channels = array();

/** protected @var array   Channel objects for each channel, by callsign */
    var $callsigns = array();

/**
 * Constructor, duh
/**/
    /* public */
    function __construct() {
        // Don't really have much to do here.  Most stuff is loaded on demand.
    }

/**
 * Legacy constructor
/**/
    /** @deprecated */ function Channel_List() {
        return $this->__construct();
    }




/**
 * Look up a channel by its callsign
/**/
    /* static */
    function &callsign($callsign) {
        #if (empty($this->callsigns)
    }

/**
 * Look up a channel by its chanid
/**/
    /* static */
    function &chanid($chanid) {
    }

}
