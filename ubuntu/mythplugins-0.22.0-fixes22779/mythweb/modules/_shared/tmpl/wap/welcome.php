<?php
/**
 * Welcome page for the WAP template
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/_shared/tmpl/wap/welcome.php $
 * @date        $Date: 2009-08-02 06:50:00 +0200 (Sun, 02 Aug 2009) $
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
