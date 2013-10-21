import QtQuick 1.1
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0

import "effects"
import "components"
import "../../skins-common/lib/components"
import "js/ComponentLoader.js" as MyJs
import "../../skins-common/lib/handlers"

/*!
 *
 *\brief The main logic component
 *
 *\ingroup qml_desktop_default
 *
 *This file represent the main loading logic of the skin. It contains a loader that loads the subsequent
 *pages over it. This allows Main.qml to keep be a consistent object to manage states and the flow of
 *information from the dce object.
 *
 *
 */
Item {
    id: qmlroot
    width:manager.appWidth
    height:manager.appHeight
    focus:true
    Keys.onTabPressed: if(activeFocus)hideUI()

    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width
    property int screensaverTimer:manager.screenSaverTimeout*1000
    property bool hideUiElements:false

    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    Component.onCompleted: {logger.userLogMsg = "Main.qml loaded in default skin";
        manager.setBoundStatus(true);
        dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
    }

    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../skins-common/fonts/Sawasdee.ttf"
    }

    Rectangle{
        id:phil
        height: parent.height
        width: parent.width
        color:"darkslategrey"
    }

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


    //! Returns the value of the param value passed in scaled to a percentage of the current width value of the application window.
    function scaleX(x){
        return x/100*manager.appWidth
    }
    //! Returns the value of the param value passed in scaled to a percentage of the current height value of the application window.
    function scaleY(y){
        return y/100*manager.appHeight
    }

    //! \warn This function is required by the qml engine.
    //! this function load pages as called from other threads or in response to user events.
    function screenchange(screenname ){
        pageLoader.source = "screens/"+screenname
        while(pageLoader.status == Component.Loading){
            console.log("Screenchange::"+pageLoader.progress)
        }

        if (pageLoader.status == Component.Ready){
            var s = String(screenname)
            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
            manager.setCurrentScreen(screenname)
        }else if (pageLoader.status==Component.Error){
            logger.userLogMsg ="Command to change to:" + screenname + " failed!"
            logger.userLogMsg = pageLoader.sourceComponent.errorString()
            screenfile = screenname
            pageLoader.source = "screens/Screen_x.qml"
        }
    }

    function hideUI(){
        //  console.log("Ui visibility check")
        if(dceplayer.mediaPlaying){
            hideUiTimer.stop()
            if(dceplayer.activeFocus){
                hideUiElements = false
                pageLoader.forceActiveFocus()
                // console.log("PageLoader item focus::"+pageLoader.focus)
                hideUiTimer.start()
            } else{
                dceplayer.forceActiveFocus()
                //  console.log("Dceplayer focus::"+dceplayer.focus)
                hideUiElements = true
            }
        }else{
            //  console.log("No local media playing.")
            if(pageLoader.activeFocus){
                glScreenSaver.forceActiveFocus()
                hideUiTimer.stop()
                hideUiElements = true
            }else{
                pageLoader.forceActiveFocus()
                hideUiElements = false
                hideUiTimer.start()
            }
        }
    }

    function updateBackground(newImage){
        appbackground.source=manager.imagePath+"ui3/"+newImage
    }

    /*! Loads component into the component popup loader

    @param componetName: String location of the component relative to main.qml
    */
    function loadComponent(componentName ){
        componentLoader.source = "components/"+componentName
        while(componentLoader.status === Component.Loading){
            //  console.log(componentLoader.progress)
        }

        if (componentLoader.status == Component.Ready){
            logger.userLogMsg ="Command to change to:" + componentName+ " was successfull"
        }else{
            logger.userLogMsg = "Command to add: " + componentName + " failed!"
            logger.userLogMsg = componentLoader.Error.toString()
        }
    }

    Timer{
        id:hideUiTimer
        interval:screensaverTimer*1000
        running: true
        repeat: true
        onTriggered: hideUI()
    }


    Image {
        id: appbackground
        source: manager.imagePath+"ui3/light_gray_texture-wallpaper-1920x1080.jpg"
        anchors.fill: parent
        opacity: 1
    }

    //==========Visual Elements
    /*Header Element*/
    DataHeader {
        id: data_header
        z:6
        opacity: pageLoader.opacity
    }

    /* c++ Phonon based Media Player*/
    MediaManager{
        id:dceplayer
        anchors.top: parent.top
        anchors.left:parent.left
        focus:true

        //  onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
        z:dceplayer.mediaPlaying ==false ? -5 : 0
        Component.onCompleted: {
            setWindowSize(manager.appHeight, manager.appWidth);
        }

        Connections{
            target:manager
            onOrientationChanged:dceplayer.setWindowSize(manager.appHeight, manager.appWidth)
            onMediaPlayerIdChanged:{
                console.log("initializing media player"+manager.mediaPlayerID)
                dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
            }
        }

        onCurrentStatusChanged:logger.logMediaMessage("Media Player Status::"+dceplayer.currentStatus)
        onMediaBufferChanged: console.log("media buffer change:"+mediaBuffer)
        onMediaPlayingChanged: {
            console.log("Media Playback status changed locally "+dceplayer.mediaBuffer);
            if(dceplayer.mediaPlaying){
                hideUI()
                forceActiveFocus()
            }else{
                hideUI()
            }
        }

        Keys.onPressed: {

            switch(event.key){
            case Qt.Key_Back:
                manager.changedPlaylistPosition((mediaplaylist.currentIndex-++1));
                break;
            case Qt.Key_Forward:
                manager.changedPlaylistPosition((mediaplaylist.currentIndex+1))
                break;
            case 16777347: /* Keycode Track forward */
                manager.changedPlaylistPosition((mediaplaylist.currentIndex+1));
                break;
            case 16777346: /* Keycode Track Backwards */
                manager.changedPlaylistPosition((mediaplaylist.currentIndex-1))
                break;
            case Qt.Key_Plus: /*Plus sign */
                manager.adjustVolume(+1)
                break;
            case Qt.Key_Tab:
                hideUI()
                break;

            case Qt.Key_VolumeMute:
                manager.mute()
                break;

            case Qt.Key_M:
                manager.mute()
                break;

            case Qt.Key_Minus: /* Minus Sign */
                manager.adjustVolume(-1)
                break;
            case Qt.Key_T:
                if(playlist.state==="showing")
                    playlist.state="hidden"
                else
                    playlist.state = "showing"
                break;
            case Qt.Key_Delete:
                dceplayer.flipColors = !dceplayer.flipColors
                break;

            case Qt.Key_S:
                manager.stopMedia()
                break;

            case Qt.Key_Pause:
                manager.pauseMedia()
                break;
            case Qt.Key_P:
                manager.pauseMedia()
                break;

            case Qt.Key_PageUp:
                manager.changedPlaylistPosition(mediaplaylist.currentIndex-1)
                break;

            case Qt.Key_PageDown:
                manager.changedPlaylistPosition(mediaplaylist.currentIndex+1)
                break;
            default:
                console.log(event.key)
                break
            }
        }
    }

    Loader {
        id:pageLoader
        objectName: "loadbot"
        focus:true
        z:5
        anchors{
            top:data_header.bottom
            bottom:qmlroot.bottom
            left:qmlroot.left
            right:qmlroot.right
        }
        onSourceChanged:  loadin
        onLoaded: {console.log("Screen Changed:" + pageLoader.source) }

        opacity: hideUiElements ? 0 : 1
        Behavior on opacity {
            PropertyAnimation{
                duration: 350
            }
        }
        Keys.onTabPressed: hideUI()
    }

    Loader{
        id:componentLoader
        anchors{
            top:qmlroot.top
            bottom:qmlroot.bottom
            left:qmlroot.left
            right:qmlroot.right
        }
        z:5
        focus:false
        objectName: "componentbot"
        onLoaded: {
            console.log("Component is loaded")
            componentLoader.z = 5
        }
    }
}
