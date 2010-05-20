<?php
/**
 * This header file is shared by all MythWeb modules.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/_shared/tmpl/default/header.php $
 * @date        $Date: 2009-08-01 21:50:00 -0700 (Sat, 01 Aug 2009) $
 * @version     $Revision: 21099 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 *
/**/

// UTF-8 content
    header("Content-Type: text/html; charset=utf-8");

// Globals
    global $headers, $page_title;

// Create the category legend popup and stick it at the end of the document for now.
    global $Categories;
    if (!empty($Categories)) {
        $legend = <<<EOF
<table width="400" style="background-color: #003060;" border="1" cellpadding="0" cellspacing="0">
<tr>
    <td><table width="400" style="background-color: #003060;" class="small" cellpadding="5" cellspacing="5">
        <tr>
EOF;
        $legend .= "\t\t\t<td colspan=\"3\">".t('Category Legend').':</td>';
        $count = 0;
        foreach ($Categories as $cat => $details) {
            if ($count++ % 3 == 0)
                $legend .= "\n\t\t</tr><tr>\n";
            $legend .= "\t\t\t<td class=\"cat_$cat\" align=\"center\"><b>".html_entities($details[0])."</b></td>\n";
        }
        $legend .= <<<EOF
        </tr>
        </table></td>
</tr>
</table>
EOF;
    }

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
    <base href="<?php echo root_url; ?>">
    <title><?php echo html_entities($page_title) ?></title>

    <link rel="icon"          href="<?php echo skin_url ?>img/favicon.ico" type="image/x-icon">
    <link rel="shortcut icon" href="<?php echo skin_url ?>img/favicon.ico" type="image/x-icon">

    <link type="application/opensearchdescription+xml" rel="search" href="tv/opensearch?type=xml" title="MythTV">

    <meta http-equiv="content-type" content="text/html; charset=utf-8">
    <meta name="robots" content="noindex, nofollow">

    <script type="text/javascript" src="js/prototype.js"></script>
    <script type="text/javascript" src="js/prototip/prototip.js"></script>
    <link rel="stylesheet" type="text/css" href="js/prototip/prototip.css">

    <script type="text/javascript" src="js/utils.js"></script>
    <script type="text/javascript" src="js/AC_OETags.js"></script>
    <script type="text/javascript" src="js/table_sort.js"></script>

    <script type="text/javascript">
        <!--
        // -----------------------------------------------------------------------------
        // Globals
        // Major version of Flash required
        var requiredMajorVersion = 9;
        // Minor version of Flash required
        var requiredMinorVersion = 0;
        // Minor version of Flash required
        var requiredRevision = 0;
        // -----------------------------------------------------------------------------
        // -->
    </script>

    <link rel="stylesheet" type="text/css" href="<?php echo skin_url ?>/style.css">
    <link rel="stylesheet" type="text/css" href="<?php echo skin_url ?>/header.css">
    <link rel="stylesheet" type="text/css" href="<?php echo skin_url ?>/menus.css">
    <link rel="stylesheet" type="text/css" href="<?php echo skin_url ?>/programming.css">

<?php
    if (!empty($headers) && is_array($headers))
        echo '    ', implode("\n    ", $headers), "\n";
?>

</head>

<body>

<div id="page_header" class="clearfix">
    <div id="logo_box">
        <a id="mythtv_logo" href="<?php echo root_url; ?>">
        <img src="<?php echo skin_url ?>img/mythtv-logo.png" alt="MythTV" class="alpha_png">
        </a>
    </div>
    <div id="sections">
<?php
    if (Modules::getModule('tv')) {
?>
        <a id="tv_link"<?php if ($Path[0] == 'tv') echo ' class="current_section"' ?> href="tv" onmouseover="return help_text('<?php echo str_replace("'", "\\'", t('TV functions, including recorded programs.')) ?>')" onmouseout="return help_text()">
            <img src="<?php echo skin_url ?>img/tv.png" class="alpha_png" alt="MythTV">
        </a>
<?php
    }
    if (Modules::getModule('music')) {
?>
        <a id="music_link"<?php if ($Path[0] == 'music') echo ' class="current_section"' ?> href="music" onmouseover="return help_text('<?php echo str_replace("'", "\\'", t('MythMusic on the web.')) ?>')" onmouseout="return help_text()">
            <img src="<?php echo skin_url ?>img/music.png" class="alpha_png" alt="MythMusic">
        </a>
<?php
      }
      if (Modules::getModule('video')) {
?>
        <a id="video_link"<?php if ($Path[0] == 'video') echo ' class="current_section"' ?> href="video" onmouseover="return help_text('<?php echo str_replace("'", "\\'", t('MythVideo on the web.')) ?>')" onmouseout="return help_text()">
            <img src="<?php echo skin_url ?>img/video.png" class="alpha_png" alt="MythVideo">
        </a>
<?php
      }
      if (Modules::getModule('weather')) {
?>
        <a id="weather_link"<?php if ($Path[0] == 'weather') echo ' class="current_section"' ?> href="weather" onmouseover="return help_text('<?php echo str_replace("'", "\\'", t('MythWeb Weather.')) ?>')" onmouseout="return help_text()">
            <img src="<?php echo skin_url ?>img/weather.png" class="alpha_png" alt="MythWeather">
        </a>
<?php
      }
?>
        <a id="settings_link"<?php if ($Path[0] == 'settings') echo ' class="current_section"' ?> href="settings" onmouseover="return help_text('<?php echo str_replace("'", "\\'", t('Edit MythWeb and some MythTV settings.')) ?>')" onmouseout="return help_text()">
            <img src="<?php echo skin_url ?>img/settings.png" class="alpha_png" alt="<?php echo t('Settings') ?>">
        </a>
    </div>
    <div id="extra_header">
        <div id="help_wrapper">
            <div id="help_box">
                <div id="help_text_default" style="position:relative;">
                MythWeb: <?php echo strftime($_SESSION['date_statusbar'], time()) ?>
                </div>
                <div id="help_text" style="display:none;">
                </div>
            </div>
        </div>
        <?php if (Modules::getModule('tv')) { ?>
        <div id="search">
            <form action="tv/search" method="get">
                <div id="simple_search">
                    <input type="hidden" name="type" value="q">
                    <input id="search_text" type="text" name="s" size="15" value="<?php echo html_entities($_SESSION['search']['s']) ?>">
                    <input id="search_submit" type="submit" class="submit" name="search" value="<?php echo t('Search') ?>">
                    (<a href="tv/search"><?php echo t('Advanced') ?></a>)
                </div>
            </form>
        </div>
        <?php } ?>
    </div>
</div>


<table width="100%" border="0" cellspacing="2" cellpadding="0">
<tr>

    <td colspan="2" class="menu menu_border_t menu_border_b"><table class="body" width="100%" border="0" cellspacing="2" cellpadding="2">
        <tr>
            <td><div id="command_choices">
                    <a href="" <?php
                        echo show_popup('category_legend',$legend)
                        ?>>MythTV:</a> &nbsp; &nbsp;
                    <?php if (Modules::getModule('tv')) { ?>
                        <a href="tv/list"><?php echo t('Listings') ?></a>
                        &nbsp; | &nbsp;
                        <a href="tv/searches"><?php echo t('Searches') ?></a>
                        &nbsp; | &nbsp;
                        <a href="tv/schedules"><?php echo t('Recording Schedules') ?></a>
                        (<a href="tv/schedules/manual"><?php echo t('Manual') ?></a>,
                        <a href="tv/schedules/custom"><?php echo t('Custom') ?></a>)
                        &nbsp; | &nbsp;
                        <a href="tv/upcoming"><?php echo t('Upcoming Recordings') ?></a>
                        &nbsp; | &nbsp;
                        <a href="tv/recorded"><?php echo t('Recorded Programs') ?></a>
                        &nbsp; | &nbsp;
                    <?php } ?>
                    <a href="status"><?php echo t('Backend Status') ?></a>
<?php if (Modules::getModule('backend_log')) { ?>
                    &nbsp; | &nbsp;
                    <a href="backend_log"><?php echo t('Backend Logs') ?></a>
<?php } ?>
                </div></td>
        </tr>
        </table></td>

</tr>
</table>

<?php
// Errors go here
    display_errors();
