<?php
/**
 * Display template for Static Maps for the Weather module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/weather/tmpl/default/weather.sm.php $
 * @date        $Date: 2009-09-12 06:26:24 +0200 (Sat, 12 Sep 2009) $
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
