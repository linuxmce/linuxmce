<?php
/**
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/tv/tmpl/iPod/list_shows_in_title_and_group.php $
 * @date        $Date: 2009-08-02 06:50:00 +0200 (Sun, 02 Aug 2009) $
 * @version     $Revision: 21099 $
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
        if (is_array($SubTitles)) {
            foreach ($SubTitles as $subtitle) {
                echo '<li><a href="'.root_url.'tv/detail/'.urlencode($subtitle[1]).'/'.urlencode($subtitle[2]).'">'.$subtitle[0]."\n";
            }
        }
    ?>
</ul>

<?php
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
