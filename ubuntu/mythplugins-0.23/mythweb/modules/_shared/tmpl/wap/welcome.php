<?php
/**
 * Welcome page for the WAP template
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/_shared/tmpl/wap/welcome.php $
 * @date        $Date: 2009-08-01 21:50:00 -0700 (Sat, 01 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythTV
 *
/**/

   require_once 'modules/_shared/tmpl/'.tmpl.'/header.php';
?>

<a id="reset" href="<?php echo root_url; ?>?RESET_SKIN&RESET_TMPL"><?php echo t('Reset template and skin to defaults'); ?></a>

<?php
   require_once 'modules/_shared/tmpl/'.tmpl.'/footer.php';
