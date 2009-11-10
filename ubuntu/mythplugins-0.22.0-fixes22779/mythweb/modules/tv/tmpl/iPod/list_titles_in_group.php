<?php
/**
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/tv/tmpl/iPod/list_titles_in_group.php $
 * @date        $Date: 2009-08-09 08:05:07 +0200 (Sun, 09 Aug 2009) $
 * @version     $Revision: 21182 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
/**/

// Set the desired page title
    $page_title = 'MythWeb - ' . t('Recording Titles');

    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'ListPanel.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<ul class="ListPanel">
    <?php
        if (is_array($Titles)) {
            foreach ($Titles as $title) {
                echo '<li><a href="'.root_url.'tv/list_shows_in_title_and_group/?group='.urlencode($group).'&title='.urlencode($title[0]).'">'.$title[0].($title[1] > 1 ? " ({$title[1]})" : '')."\n";
            }
        }
    ?>
</ul>

<?php
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
?>
