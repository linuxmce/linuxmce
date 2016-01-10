import QtQuick 1.1
import AudioVisual 1.0
import DceScreenSaver 1.0
import org.linuxmce.screens 1.0
import org.linuxmce.enums 1.0

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

    Style{
        id:skinStyle
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
        x:parent.x
        y:parent.y
        interval:30000
        useAnimation: true
        onDebugInfoChanged: console.log(debugInfo)
        active:manager.m_ipAddress==="192.168.80.1"
        requestUrl:manager.m_ipAddress
        Component.onCompleted: {
            glScreenSaver.setImageList(manager.screensaverImages)
        }

        Connections{
            target:manager
            onScreenSaverImagesReady:{
                glScreenSaver.setImageList(manager.screensaverImages)
                console.log("Orbiter Consume Screensaver images")
                console.log("Orbiter counts " + glScreenSaver.pictureCount)
            }
        }


        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onPressed:if(glScreenSaver.activeFocus) hideUI()
        }

    }


    //! Returns the value of the param value passed in scaled to a percentage of the current width value of the application orbiterWindow.
    function scaleX(x){
        return x/100*manager.appWidth
    }
    //! Returns the value of the param value passed in scaled to a percentage of the current height value of the application orbiterWindow.
    function scaleY(y){
        return y/100*manager.appHeight
    }

    //! \warn This function is required by the qml engine.
    //! this function load pages as called from other threads or in response to user events.
    function screenchange(screenname ){
        console.log("New Screen Request::"+screenname)
        if(screenname==""){
            console.log("Empty Screen!")
            return;
        }

        pageLoader.source = "screens/"+screenname
        //        while(pageLoader.status == Component.Loading){
        //            console.log("Screenchange::"+pageLoader.progress)
        //        }

        if (pageLoader.status == Component.Ready){
            var s = String(screenname)
            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
            // manager.setCurrentScreen(screenname)
            componentLoader.source= ""
        }else if (pageLoader.status==Component.Error){
            manager.setCurrentScreen("Error.qml")
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
    function loadComponent(componentName, paramObj ){

        console.log("Loading Component " + componentName)
        if(componentName===""){
            componentLoader.source=""
            return
        }

        if(paramObj){
            componentLoader.currentParams = paramObj
        }

        componentLoader.source = "components/"+componentName



        if (componentLoader.status == Component.Ready){
            if(paramObj){
                var i = componentLoader.item
                for(var p in paramObj){
                    i[p] =paramObj[p]
                }
                console.log(JSON.stringify(componentLoader.item))
            }
            logger.userLogMsg ="Command to change to:" + componentName+ " was successfull"
        }else if (componentLoader.status===Component.Error){
            logger.userLogMsg = "Command to add: " + componentName + " failed!"
            logger.userLogMsg = componentLoader.Error.toString()
        } else if (componentLoader.status===Component.Loading){
            console.log("Component Still loading..")
        }
    }

    Timer{
        id:hideUiTimer
        interval:screensaverTimer*1000
        running: true
        repeat: true
        onTriggered: hideUI()
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
        useInvertTrick: false


        //  onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
        z:-5
        onVideoStreamChanged: {
            if(videoStream){
                console.log("dceplayer::raising orbiterWindow")
                dceplayer.z=1
                glScreenSaver.setScreenSaverActive(false)
            } else {
                console.log("dceplayer::lowering orbiterWindow")
                glScreenSaver.setScreenSaverActive(true)
                dceplayer.z=-5
            }
        }

        Component.onCompleted: {
            setorbiterWindowSize(manager.appHeight, manager.appWidth);
        }

        Connections{
            target:manager
            onOrientationChanged:dceplayer.setorbiterWindowSize(manager.appHeight, manager.appWidth)
            onMediaPlayerIdChanged:{
                console.log("initializing media player"+manager.mediaPlayerID)
                dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
            }
            onConnectedStateChanged:{
                if(manager.connectedState){
                    dceplayer.reInit()
                }
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
                manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition-1));
                break;
            case Qt.Key_Forward:
                manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition+1))
                break;
            case 16777347: /* Keycode Track forward */
                manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition+1));
                break;
            case 16777346: /* Keycode Track Backwards */
                manager.changedPlaylistPosition((dcenowplaying.m_iplaylistPosition-1))
                break;
            case Qt.Key_Plus: /*Plus sign */
                manager.adjustVolume(+1)
                break;
            case Qt.Key_Tab:
                hideUI()
                break;
            case Qt.Key_F3:
                manager.clearSkinCache()
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
                console.log("Didnt handle::"+event.key)
                break
            }
        }
    }

    Loader {
        id:pageLoader
        objectName: "loadbot"
        focus:true
        z:5
        source: "screens/Screen_"+Screens.Main+".qml"
        anchors{
            top:data_header.bottom
            bottom:qmlroot.bottom
            left:qmlroot.left
            right:qmlroot.right
        }

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
        property variant currentParams:null
        objectName: "componentbot"
        onLoaded: {
            console.log("Component is loaded")

            if(currentParams!==null){

                var i = componentLoader.item

                for(var p in currentParams){
                    console.log(JSON.stringify(currentParams))
                    console.log(JSON.stringify(p))
                    i[p] =currentParams[p]
                }

                console.log("Set item parameters")
                currentParams=null
            }
            componentLoader.z = 5
        }
    }
}
