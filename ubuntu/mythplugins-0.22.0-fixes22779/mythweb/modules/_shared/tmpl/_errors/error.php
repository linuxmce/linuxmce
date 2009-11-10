<?php
/**
 *
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/_shared/tmpl/_errors/error.php $
 * @date        $Date: 2009-09-16 07:07:14 +0200 (Wed, 16 Sep 2009) $
 * @version     $Revision: 21885 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage
 *
/**/

// Set the desired page title
    $page_title = 'MythWeb - Error';

// Custom headers
    $headers[] = '<link rel="stylesheet" type="text/css" href="skins/errors.css">';

// Print the page header
    @include 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<div id="message">

<h2><?php echo htmlentities($header, ENT_COMPAT, 'UTF-8') ?></h2>

<p>
<?php echo nl2br(htmlentities($text, ENT_COMPAT, 'UTF-8')) ?>
</p>

</div>

<?php

// Print the page footer
    @include 'modules/_shared/tmpl/'.tmpl.'/footer.php';
