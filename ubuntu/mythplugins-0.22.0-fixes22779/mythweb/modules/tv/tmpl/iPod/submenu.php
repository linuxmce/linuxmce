<?php
/**
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/tv/tmpl/iPod/submenu.php $
 * @date        $Date: 2009-01-22 08:08:37 +0100 (Thu, 22 Jan 2009) $
 * @version     $Revision: 19784 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
/**/

// Set the desired page title
    $page_title = 'MythWeb - ' . t('TV Submenu');

    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'/AppPanel.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<div id="AppPanel">
    <?php
        foreach ($SubModules as $submodule) {
            ?>
            <div class="appicon">
                <a href="<?php echo $submodule['URL']; ?>">
                    <img src="<?php echo $submodule['Icon']; ?>.png"><br>
                    <?php echo $submodule['Name']; ?>
                </a>
            </div>
            <?php
        }
    ?>
</div>

<?php
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
?>
