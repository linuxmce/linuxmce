<?php
/**
 *
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/_shared/tmpl/_errors/lockdown.php $
 * @date        $Date: 2009-08-01 21:50:00 -0700 (Sat, 01 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage
 *
/**/

// Set the desired page title
    $page_title = 'MythWeb - '.t('Error');

// Custom headers
    $headers[] = '<link rel="stylesheet" type="text/css" href="skins/errors.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<div id="message">

<h2><?php echo t('MythWeb Locked'); ?></h2>

<p>
<?php
    echo t('LOCKDOWN_NOTICE').' '.realpath('lockdown').' '.t('LOCKDOWN_NOTICE_2');
?>
</p>

</div>

<?php

// Print the page footer
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
