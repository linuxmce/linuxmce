<?php
/**
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/remote/tmpl/iPod/play_program_on_frontend.php $
 * @date        $Date: 2009-08-01 21:50:00 -0700 (Sat, 01 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Set the desired page title
    $page_title = 'MythWeb - ' . t('Play Recording on Frontend');

    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'ListPanel.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<ul class="ListPanel">
    <?php
        foreach (MythFrontend::findFrontends() as $frontend)
            echo '<li><a href="remote/play_program_on_frontend?host='.urlencode($frontend->getHost()).'&chanid='.urlencode($chanid).'&starttime='.urlencode($starttime).'">'.$frontend->getHost().'</a>';
    ?>
</ul>


<?php
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
