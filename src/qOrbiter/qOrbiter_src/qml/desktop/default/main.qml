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
        height: parent.height
        width: parent.width
        interval:8000
        anchors.centerIn: parent
        requestUrl:manager.m_ipAddress
        Connections{
            target:manager
            onScreenSaverImagesReady:glScreenSaver.setImageList(manager.screensaverImages)
        }

    }


    //    PropertyAnimation{
    //        target:glScreenSaver
    //        property:"rotation"
    //        loops: Animation.Infinite
    //        to:360
    //        duration: 1000
    //        running:true
    //    }

    /*! Depreciated function.
      Was previously used to deal with orientation changes.
      */
    function checkLayout()
    {
        console.log("c++ slot orientation changed")
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
    function screenchange(screenname )
    {
        pageLoader.source = "screens/"+screenname
        while(pageLoader.status == Component.Loading)
        {
            console.log(pageLoader.progress)
        }

        if (pageLoader.status == Component.Ready)
        {
            var s = String(screenname)
            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
            console.log("eggs")
            manager.setCurrentScreen(screenname)
        }
        else if (pageLoader.status==Component.Error)
        {
            logger.userLogMsg ="Command to change to:" + screenname + " failed!"
            logger.userLogMsg = pageLoader.sourceComponent.errorString()
            screenfile = screenname
            pageLoader.source = "screens/Screen_x.qml"
        }
    }

    function hideUI(){
        console.log("Ui visibility check")
        if(dceplayer.mediaPlaying)
        {
            if(dceplayer.activeFocus){
                 pageLoader.forceActiveFocus()
                console.log("PageLoader item focus::"+pageLoader.focus)
            } else{
                dceplayer.forceActiveFocus()               
                console.log("Dceplayer focus::"+dceplayer.focus)
            }
        }
        else{
            console.log("No local media playing.")
            pageLoader.forceActiveFocus()
        }
    }

    function updateBackground(newImage){
        appbackground.source=manager.imagePath+"ui3/"+newImage
    }

    function loadComponent(componentName )
    {
        componentLoader.source = "components/"+componentName
        while(componentLoader.status === Component.Loading){
            console.log(componentLoader.progress)
        }

        if (componentLoader.status == Component.Ready)
        {
            logger.userLogMsg ="Command to change to:" + componentName+ " was successfull"
        }
        else
        {
            logger.userLogMsg = "Command to add: " + componentName + " failed!"
            logger.userLogMsg = componentLoader.Error.toString()
        }
    }


    Connections{
        target: manager
        onOrientationChanged: checkLayout()
    }

        Image {
            id: appbackground
            source: manager.imagePath+"ui3/light_gray_texture-wallpaper-1920x1080.jpg"
            anchors.fill: parent
            opacity: 1
        }

        //==========Visual Elements
        DataHeader {
            id: data_header
            z:6
        }


        MediaManager{
            id:dceplayer
            anchors.top: parent.top
            anchors.left:parent.left
            focus:true

            onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
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
                }
                else{
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
            height: manager.appHeight - data_header.height
            width: manager.appWidth
            anchors.top: data_header.bottom
            onSourceChanged:  loadin
            onLoaded: {
                console.log("Screen Changed:" + pageLoader.source)
            }
            visible: !dceplayer.focus
            z:5
            Keys.onTabPressed: hideUI()
        }


        Loader{
            id:componentLoader
            height: parent.height
            width: parent.width
            objectName: "componentbot"
            onLoaded: {
                console.log("Component is loaded")
                componentLoader.z = 5
            }
            z:5
            focus:false
        }




}
