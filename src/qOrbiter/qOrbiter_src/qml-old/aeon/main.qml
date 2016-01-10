import QtQuick 1.1
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
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
    width: manager.appWidth
    height: manager.appHeight

    function scaleX(x){
        return x/100*width
    }
    function scaleY(y){
        return y/100*height
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

    Rectangle {
        id:stage

        clip: true;
        signal swapStyle()
        height: manager.appHeight
        width: manager.appWidth
        color:"transparent"

        DceScreenSaver{
            id:glScreenSaver
            height: manager.appHeight
            width: manager.appWidth
            focus:true
            interval:8000
            anchors.centerIn: parent
            requestUrl:manager.m_ipAddress
            Connections{
                target:manager
                onScreenSaverImagesReady:glScreenSaver.setImageList(manager.screensaverImages)
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onPressed:if(glScreenSaver.activeFocus) hideUI()

            }

        }
        Loader {
            id:pageLoader
            anchors.fill: parent
            objectName: "loadbot"
            source: "Splash.qml"
            onSourceChanged:  loadin
            focus: true
            onLoaded: {

                console.log("Screen Changed:" + pageLoader.source)

            }
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


}
