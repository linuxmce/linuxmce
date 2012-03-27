import QtQuick 1.0
import "components"
//import "../js/ComponentLoader.js" as MyJs
import "js/dateFormat.js" as DateFunctions

Item {
    id: item
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    property string locationinfo: "standby"
    property string screenfile
    property string screenerror
    property int mainMenuIndex

    // Style {id:style}
    width: style.orbiterW;
    height: style.orbiterH;

    function scaleX(x){
        return x/100*style.orbiterW
    }
    function scaleY(y){
        return y/100*style.orbiterH
    }

    function screenchange(screenname )
    {
        pageLoader.source = "screens/"+screenname
        if (pageLoader.status == 1)
        {
            console.log("Command to change to:" + screenname+ " was successfull")
        }
        else
        {
            console.log("Command to change to:" + screenname + " failed!")
            screenfile = screenname
            //screenerror = pageLoader.item.e
            pageLoader.source = "screens/Screen_x.qml"
        }
    }
    property int currentBGimg: 0
    function getBGimage(){
        return "image://listprovider/screensaver/"+securityvideo.timestamp;
    }
    function changeBGimage(){
        var newSource = getBGimage()
        var currentImg=imgBg1, newImg=imgBg2;
        if (imgBg2.opacity == 1) {
            currentImg = imgBg2;
            newImg = imgBg1;
        }
        newImg.source = newSource;
        //simulated random zoom function
        //origin seems to be the bottom middle of the scaled image (not the image element itself)
        //can maybe do some x,y move here to simulate a pan, but it will have to be within the contsraints of the scaled image
        //to avoid negative clipping (panning the edge of the image into the image element bounds)
        var temp = Math.random() * .25 + 1; // max scale factor is 1.25
        var from, to;
        if(Math.random() > .5) { // 50% choice between zoom in and out
            from = 1.0;
            to = temp;
        } else {
            from = temp;
            to = 1.0;
        }
        if (imgBg2.opacity == 1) {
            zoomimgBg1.from = from;
            zoomimgBg1.to = to;
            zoomimgBg1.start();
        } else {
            zoomimgBg2.from = from;
            zoomimgBg2.to = to;
            zoomimgBg2.start();
        }

        newImg.opacity = 1;
        currentImg.opacity = 0;
    }
    // Load some custom fonts
    FontLoader {
        id: scout
        source: "../aeon/fonts/scout.ttf"
    }
    FontLoader {
        id: scoutLight
        source: "../aeon/fonts/scoutlight.ttf"
    }
    FontLoader {
        id: scoutCond
        source: "../aeon/fonts/scoutcond.ttf"
    }
    FontLoader {
        id: aeonRss
        source: "../aeon/fonts/aeon_rss.ttf"
    }
    FontLoader {
        id: aeonNowPlaying
        source: "../aeon/fonts/aeon_player.ttf"
    }
    FontLoader {
        id: aeonNowPlayingCE
        source: "../aeon/fonts/aeon_player_ce.ttf"
    }
    FontLoader {
        id: aeonEdit
        source: "../aeon/fonts/aeon_edit.ttf"
    }
    FontLoader {
        id: aeonEditCE
        source: "../aeon/fonts/aeon_edit_ce.ttf"
    }
    FontLoader {
        id: aeonSettings
        source: "../aeon/fonts/aeon_settings.ttf"
    }
    Timer { // Simulate a simple PhotoScreensaver
        id: ssTimer;
        interval: 15000;
        running: false;
        repeat: true
        onTriggered: changeBGimage()
    }
    Rectangle {
        id:stage

        clip: true;
        signal swapStyle()
        height: style.orbiterH
        width: style.orbiterW
        opacity: 1

        Rectangle{
            id:bgrect
            anchors.fill: parent
            z:0
            color: "black"
            Image {
                id: imgBg1
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectCrop
                //source: getBGimage()
                anchors.fill: parent
                opacity: 1
                smooth: true
                Behavior on opacity {
                    PropertyAnimation{duration:3000}
                }
                PropertyAnimation { id: zoomimgBg1; target: imgBg1; property: "scale"; from: 1.0; to: 1.25; duration: 18000 }
            }
            Image {
                id: imgBg2
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectCrop
                //source: getBGimage()
                anchors.fill: parent
                opacity: 0
                smooth: true
                Behavior on opacity {
                    PropertyAnimation{duration:3000}
                }
                PropertyAnimation { id: zoomimgBg2; target: imgBg2; property: "scale"; from: 1.0; to: 1.25; duration: 18000 }
            }

        }
    }


    Loader {
        id:pageLoader
        objectName: "loadbot"
        source: "Splash.qml"
        onSourceChanged:  loadin
        focus: true
        onLoaded: {

            console.log("Screen Changed:" + pageLoader.source)

        }
    }

    SequentialAnimation{
        id:loadin

        PropertyAnimation{
            id:fadeout
            target:pageLoader
            properties: "opacity"; to: "0"; duration: 5000

        }
        PropertyAnimation{
            id: fadein
            target:pageLoader
            properties: "opacity"; to: "1"; duration: 5000
        }

    }

    Component.onCompleted: {
        changeBGimage();
        ssTimer.start();
    }
}
