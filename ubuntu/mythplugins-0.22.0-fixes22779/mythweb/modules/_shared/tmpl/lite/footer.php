<?php
/**
 * This footer file is shared by all MythWeb modules.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/_shared/tmpl/lite/footer.php $
 * @date        $Date: 2007-12-31 21:38:38 +0100 (Mon, 31 Dec 2007) $
 * @version     $Revision: 15274 $
 * @author      $Author: xris $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// Display footnotes
    global $Footnotes;
    if (is_array($Footnotes)) {
        foreach ($Footnotes as $note) {
            echo $note;
        }
    }
?>

</body>
</html>
