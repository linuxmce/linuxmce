<?php
/**
 *
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/_shared/tmpl/_errors/db_access_denied.php $
 * @date        $Date: 2009-09-16 07:02:29 +0200 (Wed, 16 Sep 2009) $
 * @version     $Revision: 21884 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage
 *
/**/

// Set the desired page title
    $page_title = 'MythWeb - Error - Database Access Denied';

// Custom headers
    $headers[] = '<link rel="stylesheet" type="text/css" href="skins/errors.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<div id="message">

<h2>Database Access Denied</h2>

<p>You are most likely receiving this message because you have failed to configure mythweb\'s database login info.</p>

<p>Please see INSTALL for instructions.</p>

</div>

<?php
// Print the page footer
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
