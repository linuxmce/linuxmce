<?php
/**
 * Initialization routines for the MythWeb Weather module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/weather/init.php $
 * @date        $Date: 2008-02-18 01:50:28 +0100 (Mon, 18 Feb 2008) $
 * @version     $Revision: 16122 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/

// First, we check if the table exists...
    if(tmpl == 'default' && $db->query_num_rows('SHOW TABLES LIKE "weatherscreens"')) {
    // Settings options
        $Settings['weather'] = array('name'    => t('Weather'),
                                     'choices' => array('screen' => 'Screen Settings'),
                                     'default' => 'screen',
                                    );
    // we should check to see that MythWeather is configured.
        $has_weather = $db->query_col('SELECT COUNT(screen_id)
                                         FROM weatherscreens');
    // And also check that the weather scripts exist on this host
        if ($has_weather) {
            $paths = $db->query_list('SELECT path
                                      FROM   weathersourcesettings
                                      WHERE  hostname=?',
                                     hostname);
            foreach ($paths as $path ) {
                if (!file_exists($path)) {
                    $has_weather = false;
                    break;
                }
            }
        }
    }

// If weather is enabled, add it to the list.
    if ($has_weather) {
        $Modules['weather'] = array('path'        => 'weather',
                                    'sort'        => 4,
                                    'name'        => t('Weather'),
                                    'description' => t('Local weather forecast')
                                   );
    }
