<?php
/**
 * Display template for Six Day Forecast for the Weather module
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/weather/tmpl/default/weather.6d.php $
 * @date        $Date: 2008-02-22 22:35:09 +0100 (Fri, 22 Feb 2008) $
 * @version     $Revision: 16202 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/
?>
    <p class="host"><?php
        echo t('Host') .": ".  $screen->host;
    ?></p>
    <p class="location"><?php
        echo $screen->data["6dlocation"]
    ?></p>

    <div class="forecast clearfix">
        <h2><?php echo $screen->container ?></h2>

        <?php
            for($i=0;$i<6;$i++) {
        ?>

        <div class="daily_forecast">
            <h3><?php echo $screen->data["date-$i"] ?></h3>
            <img src="<?php echo skin_url ?>img/weather/<?php echo $screen->data["icon-$i"] ?>" class="alpha_png" alt="">

            <div class="temps">
                <div class="low">
                    <p><?php echo t('Low') ?></p>
                    <p class="temp">
                        <?php echo $screen->data["low-$i"] == 'NA' ? '' : $screen->data["low-$i"] ?>&deg;<sup><?php echo $screen->units == 0 ? 'C' : 'F' ?></sup>
                    </p>
                </div>

                <div class="high">
                    <p><?php echo t('High') ?></p>
                    <p class="temp">
                        <?php echo $screen->data["high-$i"] == 'NA' ? '' : $screen->data["high-$i"] ?>&deg;<sup><?php echo $screen->units == 0 ? 'C' : 'F' ?></sup>
                    </p>
                </div>
            </div>
        </div>
        <?php } ?>
    </div>
