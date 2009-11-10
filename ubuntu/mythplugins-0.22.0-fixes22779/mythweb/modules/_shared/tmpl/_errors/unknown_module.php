<?php
/**
 *
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/_shared/tmpl/_errors/unknown_module.php $
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
    $page_title = 'MythWeb - Error';

// Custom headers
    $headers[] = '<link rel="stylesheet" type="text/css" href="skins/errors.css">';

// Print the page header
    require 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<div id="message">

<h2><An unknown module was specified</h2>

<p>
<?php echo $Path[0] ?>
</p>

</div>

<?php
// Print the page footer
    require 'modules/_shared/tmpl/'.tmpl.'/footer.php';
