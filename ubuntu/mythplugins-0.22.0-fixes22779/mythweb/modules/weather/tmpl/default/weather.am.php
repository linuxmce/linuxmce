<?php
/**
 * Display template for Animated Maps for the Weather module
 a
 * @url         $URL: http://svn.mythtv.org/svn/branches/release-0-22-fixes/mythplugins/mythweb/modules/weather/tmpl/default/weather.am.php $
 * @date        $Date: 2009-09-12 06:26:24 +0200 (Sat, 12 Sep 2009) $
 * @version     $Revision: 21795 $
 * @author      $Author: kormoc $
 * @license     GPL
 *
 * @package     MythWeb
 * @subpackage  Weather
 *
/**/
?>
    <div class="radar">
        <h2><?php echo $screen->data["amdesc"] ?></h2>

        <div class="radar_image"><center>
        <script type='text/javascript'>
        <!--

        images  = new Array();
        imageNum = 0;
        speed = 3;
        delay = 300;
<?php
            $matches = array();
            $image = array_pop(split('/', $screen->data["animatedimage"]));
            preg_match("/(.*)-\%1-(\d*)-(\d*)x(\d*)/", $image, $matches);

            echo "imageTotal = ". $matches[2] .";\n";
            for ($i=0; $i<$matches[2]; $i++)  {
                echo "images[$i] = new Image();\n";
                echo "images[$i].src = \"data/". $matches[1] ."-$i\";\n";
            }
?>
        function nextFrame (inc) {
            // display next frame from images array
            imageNum += inc;
            if (imageNum >= imageTotal) {
                imageNum = 0;
            }

            imageCurrent = imageNum;
            document.animation.src = images[imageCurrent].src;
        }

        function animate() {
            // start animation loop
            nextFrame(1);
        }

        function setDelay(i) {
            speed = i;
            delay =  speed * 100;
            if (delay == 0) delay = 50;
        }

        output  = '<form name="animap" id="animap" action="get">'
        output += '<b>Slower</b> '
<?php
        for ($i=6;$i>=0;$i-=1)  {
            echo "output += '<input type=\"radio\" name=\"speedRadio\" value=\"". $i. "\" onClick=\"setDelay(". $i .")\"";
            if ($i == 3) { echo " checked"; }
            echo ">'\n";
        }
?>

        output += ' <b>Faster</b><br>'
        output += '<img name="animation" src="<?php echo 'data/'. $matches[1] ."-0" ?>" alt="Animation" onload="setTimeout(\'animate()\', delay)">'
        output += '</form>'
        document.write(output);

        // -->
        </script>
        </center>
        </div>
    </div>
