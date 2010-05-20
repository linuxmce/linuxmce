<?php
/**
 * Create a rss of the recorded programs.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/tv/tmpl/rss/recorded.php $
 * @date        $Date: 2009-01-21 23:08:37 -0800 (Wed, 21 Jan 2009) $
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
