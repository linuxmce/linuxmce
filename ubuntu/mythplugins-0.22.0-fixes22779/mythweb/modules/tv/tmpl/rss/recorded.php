<?php
/**
 * Create a rss of the recorded programs.
 *
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/tv/tmpl/rss/recorded.php $
 * @date        $Date: 2009-01-22 08:08:37 +0100 (Thu, 22 Jan 2009) $
 * @version     $Revision: 19784 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  TV
 *
/**/

    $Feed = new FeedWriter(RSS2);
    $Feed->setTitle('MythWeb - '.t('Recorded Programs'));
    $Feed->setLink(root_url);
    $Feed->setDescription('MythWeb - '.t('Recorded Programs'));

    foreach ($All_Shows as $show) {
        $item = $Feed->createNewItem();

        $item->setTitle($show->title.(strlen($show->subtitle) > 0 ? ' - '.$show->subtitle : ''));
        $item->setLink(root_url.'tv/detail/'.$show->chanid.'/'.$show->recstartts);
        $item->setDate($show->starttime);
        $item->setDescription($show->description);

        $Feed->addItem($item);
    }

    $Feed->generateFeed();
