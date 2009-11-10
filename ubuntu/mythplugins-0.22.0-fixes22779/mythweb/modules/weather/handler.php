<?php
/**
 * Handler for the Weather module.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/weather/handler.php $
 * @date        $Date: 2008-02-17 01:07:51 +0100 (Sun, 17 Feb 2008) $
 * @version     $Revision: 16087 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/

    $WeatherScreens = array();

// Get configured WeatherScreens from the database
    $sh = $db->query('SELECT screen_id, draworder 
                      FROM weatherscreens
                      WHERE hostname=?',
                      $_SESSION['settings']['host']);

    while(list($screen_id, $draworder) = $sh->fetch_row()) {
        $WeatherScreens[$draworder] = new WeatherScreen($screen_id);
        $WeatherScreens[$draworder]->getData();
    }

    ksort($WeatherScreens);
    $sh->finish();

// Print the weather page template
    require_once tmpl_dir.'weather.php';

// Exit
    exit;

