<?php
/**
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/tv/tmpl/iPod/search.php $
 * @date        $Date: 2009-05-09 07:46:38 +0200 (Sat, 09 May 2009) $
 * @version     $Revision: 20518 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
/**/

// Set the desired page title
    $page_title = 'MythWeb - '.t('Search');

    $headers[] = '<link rel="stylesheet" type="text/css" href="'.skin_url.'ListPanel.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>



<?php
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
?>
