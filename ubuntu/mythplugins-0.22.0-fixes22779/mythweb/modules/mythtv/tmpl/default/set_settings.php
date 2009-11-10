<?php
/**
 * Configure MythTV Settings table
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/mythtv/tmpl/default/set_settings.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Settings
 *
/**/
?>
<form class="form" method="post" action="<?php echo form_action ?>">
<input type="hidden" name="host" value="<?php echo html_entities($_SESSION['settings']['host']) ?>" />

<table border="0" cellpadding="4" cellspacing="2" class="list small" align="center">
<tr class="menu" align="center">
    <td><?php echo t('Value') ?></td>
    <td><?php echo t('Data') ?></td>
    <td><?php echo t('Delete') ?></td>
</tr><?php
    foreach ($MythSettings as $value => $data) {
?><tr class="settings">
    <td align="right"><?php echo html_entities($value) ?></td>
    <td><input type="text" size="64" name="settings[<?php echo html_entities($value) ?>]" value="<?php echo html_entities($data) ?>"></td>
    <td><input type="checkbox" name="delete[<?php echo html_entities($value) ?>]" value="1"></td>
</tr><?php
    }
?>
</table>

<p align="center">
<input type="submit" class="submit" name="save" value="<?php echo t('Save') ?>">
</p>

</form>

