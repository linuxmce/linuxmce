import QtQuick 1.1
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import "effects"
import "components"
import "../lib/components"
import "js/ComponentLoader.js" as MyJs
import "../lib/handlers"

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


    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width

    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    Component.onCompleted: {logger.userLogMsg = "Main.qml loaded in default skin"; manager.setBoundStatus(true)}



    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "fonts/Sawasdee.ttf"
    }



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
            if(dceplayer.focus){
                dceplayer.focus = false
                pageLoader.forceActiveFocus()
                console.log("PageLoader item focus::"+pageLoader.focus)
            } else{
                dceplayer.forceActiveFocus()
                pageLoader.focus = false
                console.log("Dceplayer focus::"+dceplayer.focus)
            }
        }
        else{
            console.log("No local media playing.")
        }
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

    //    DebugPanel{
    //        id:dcemessages
    //        debugMessage: dcemessage
    //        z:2
    //        anchors.top: parent.top
    //    }

    //    DebugPanel{
    //        id:mediaMessages
    //        debugMessage: manager.mediaResponse
    //        z:2
    //        anchors.top: dcemessages.bottom
    //    }

    //    DebugPanel{
    //        id:commandmessages
    //        debugMessage: manager.commandResponse
    //        z:2
    //        anchors.top: mediaMessages.bottom
    //    }

    //    DebugPanel{
    //        id:mediaplayerMessages
    //        debugMessage: dceplayer.currentStatus
    //        anchors.top: commandmessages.bottom
    //        z:2
    //        color: dceplayer.connected ? "green" : "red"
    //    }

    Connections{
        target: manager
        onOrientationChanged: checkLayout()
    }



    FocusScope{
        id:mainScope
        height: manager.appHeight
        width: manager.appWidth
        focus:true
        Keys.onTabPressed: hideUI()


        ScreenSaver
        {   id:ss
            height: manager.appHeight
            width: manager.appWidth
            anchors.centerIn: parent
            Component.onCompleted: screensaver.setActive(true)
            MouseArea{
                anchors.fill: ss
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked:  Qt.RightButton ? pageLoader.visible = !pageLoader.visible: ""
            }
        }

        Image {
            id: appbackground
            source: manager.imagePath+"ui3/appBackground.png"
            anchors.fill: parent
            opacity: .25
        }

        DataHeader {
            id: data_header
            z:6

        }

        //        Rectangle{
        //            id:alternate_background
        //            anchors.fill: parent
        //            gradient: Gradient{
        //                GradientStop { position: 0.0; color: "darkslategrey" }
        //                GradientStop { position: 0.33; color: "slategrey" }
        //                GradientStop { position: 1.0; color: "black" }
        //            }

        //            focus:false
        //        }

        MediaManager{
            id:dceplayer
            anchors.top: parent.top
            anchors.left:parent.left

            onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
            z:dceplayer.mediaPlaying ==false ? -5 : 0
            Component.onCompleted: {setWindowSize(manager.appHeight, manager.appWidth); dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)}

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
            onMediaPlayingChanged: console.log("Media Playback status changed locally "+dceplayer.mediaBuffer)
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

    //-----------------------------shader related---------------------------//


    //floorplans


}
