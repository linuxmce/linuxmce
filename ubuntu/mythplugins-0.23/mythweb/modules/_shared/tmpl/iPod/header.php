<?php
/**
 * This header file is shared by all MythWeb modules.
 *
 * @url         $URL: http://svn.mythtv.org/svn/tags/release-0-23/mythplugins/mythweb/modules/_shared/tmpl/iPod/header.php $
 * @date        $Date: 2009-08-08 23:05:07 -0700 (Sat, 08 Aug 2009) $
 * @version     $Revision: 21182 $
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
?>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
    <base href="<?php echo root_url; ?>">
    <title><?php echo html_entities($page_title) ?></title>

    <meta content="width=device-width; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;" name="viewport" />

    <link rel="icon"          href="<?php echo skin_url ?>img/favicon.ico" type="image/x-icon">
    <link rel="shortcut icon" href="<?php echo skin_url ?>img/favicon.ico" type="image/x-icon">

    <meta http-equiv="content-type" content="text/html; charset=utf-8">
    <meta name="robots" content="noindex, nofollow">

    <script type="text/javascript" src="js/prototype.js"></script>

    <script type="text/javascript" src="js/utils.js"></script>
    <script type="text/javascript" src="js/table_sort.js"></script>

    <link rel="stylesheet" type="text/css" media="screen" href="<?php echo skin_url ?>style.css">
    <link rel="stylesheet" type="text/css" href="<?php echo skin_url ?>header.css">

    <?php
        if (!empty($headers) && is_array($headers))
            echo '    ', implode("\n    ", $headers), "\n";
    ?>

    <script type="text/javascript" language="javascript">
        function updateOrientation() {
            switch(window.orientation) {
                case 0:
                case 180:
                    document.body.removeClassName('landscape');
                    break;

                case 1: // android (G1)
                case 90:
                case -90:
                    document.body.addClassName('landscape');
                    break;
            }
        }

    // Hide safari's location bar by default;
        setTimeout(function(){ window.scrollTo(0, 1);} , 100);
    // Handle resize/rotate events
        Event.observe(document.onresize ? document : window,
                      "resize", updateOrientation);
    </script>

</head>

<body onorientationchange="updateOrientation();">

    <script type="text/javascript" language="javascript">
    // We need this here to get the Orientation as soon as the body tag is rendered to the dom.
        updateOrientation();
    </script>

    <div id="Header">
        <div id="PageTitle"><?php echo _or($Page_Title_Short, 'MythWeb') ?></div>
        <?php
            if (isset($Page_Previous_Location) && isset($Page_Previous_Location_Name)) {
                ?>
                <a id="homeButton" class="button" href="<?php echo $Page_Previous_Location ?>"><?php echo $Page_Previous_Location_Name ?></a>
                <?php
            }
            ?>
        <a id="Search" class="button" href="tv/search">Search</a>
    </div>
