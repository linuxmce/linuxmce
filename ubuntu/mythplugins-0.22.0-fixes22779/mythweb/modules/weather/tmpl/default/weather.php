<?php
/**
 * Display template for the Weather module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/weather/tmpl/default/weather.php $
 * @date        $Date: 2009-08-02 06:50:00 +0200 (Sun, 02 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/

// Page title
    $page_title = 'MythWeb - '.t('Weather');

// Load this page's custom stylesheet
    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'/weather.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';

// Print Information for each of the screensa
?>
<div class="weather_site">
<?php
    foreach ($WeatherScreens as $screen) {
        if ($screen->container == 'Current Conditions')
            require tmpl_dir.'weather.cc.php';
        if ($screen->container == '18 Hour Forecast')
            require tmpl_dir.'weather.18h.php';
        if ($screen->container == 'Three Day Forecast')
            require tmpl_dir.'weather.3d.php';
        if ($screen->container == 'Six Day Forecast')
            require tmpl_dir.'weather.6d.php';
        if ($screen->container == 'Static Map')
            require tmpl_dir.'weather.sm.php';
        if ($screen->container == 'Animated Map')
            require_once tmpl_dir.'weather.am.php';
    }
?>
</div>

<?php

// Print the page footer
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
