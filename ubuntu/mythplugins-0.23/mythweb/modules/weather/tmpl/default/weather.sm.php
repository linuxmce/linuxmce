<?php
/**
 * Display template for Static Maps for the Weather module
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/weather/tmpl/default/weather.sm.php $
 * @date        $Date: 2009-09-11 21:26:24 -0700 (Fri, 11 Sep 2009) $
 * @version     $Revision: 21795 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/
?>

<?php
    $image = array_pop(split('/', $screen->data['map']));
    $matches = array();
    preg_match('/(.*)-\d*x\d*/', $image, $matches);
?>



    <div class="radar">
        <h2><?php echo $screen->data["smdesc"] ?></h2>

        <div class="radar_image">
            <center>
                <img name="static_map" src="<?php echo 'data/'. $matches[1] ?>" alt="Static Map">
            </center>
        </div>
    </div>
