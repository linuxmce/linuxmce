<?php
/**
 * Configure MythWeb Session info
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/tv/tmpl/default/set_screens.php $
 * @date        $Date: 2008-07-06 19:29:45 +0200 (Sun, 06 Jul 2008) $
 * @version     $Revision: 17725 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/
?>

<form class="form" method="post" action="<?php echo form_action ?>">
    <?php
        foreach ($Screens as $screen => $options) {
            ?>
                <fieldset style="margin: 1em;">
                    <legend><?php echo t(ucwords($screen)); ?></legend>
                    <?php
                        foreach($options as $option) {
                            echo '<input type="checkbox" name="'.$screen.'['.$option.']" id="'.$screen.'['.$option.']" ';
                            if ($_SESSION['settings']['screens']['tv'][$screen][$option] == 'on')
                                echo 'checked';
                            echo '> <label for="'.$screen.'['.$option.']">'.t(ucwords($option)).'</label><br />';
                        }
                    ?>
                </fieldset>
            <?php
        }
    ?>

    <div style="width: 100%; padding-top: 2em; clear: both; text-align: center;">
        <input class="submit" type="reset" value="<?php echo t('Reset') ?>">
        &nbsp;
        <input class="submit" type="submit" name="save" value="<?php echo t('Save') ?>">
    </div>

</form>
