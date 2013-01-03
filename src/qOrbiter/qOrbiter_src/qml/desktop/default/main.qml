import QtQuick 1.1
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import "effects"
import "components"
import "lib/components"
import "js/ComponentLoader.js" as MyJs
import "lib/handlers"



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
    id: item
    width:manager.appWidth
    height:manager.appHeight
    property alias skinStyle: style

    WaitSpinner {
        id: waitSpinner
        anchors.centerIn: parent
    }

    Style{
       id:style
   }
    Component.onCompleted: {logger.userLogMsg = "Main.qml loaded in default skin"; manager.setBoundStatus(true)}

    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "fonts/Sawasdee.ttf"
    }

    MediaManager{
        id:dceplayer
        anchors.top: parent.top
        anchors.left:parent.left
        z:dceplayer.mediaPlaying ==false ? -5 : 0
        Component.onCompleted: {setWindowSize(manager.appHeight, manager.appWidth); dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)}
        MouseArea{
            anchors.fill: dceplayer
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked:  Qt.RightButton ? pageLoader.visible = !pageLoader.visible: ""
        }
        Connections{
            target:manager
            onOrientationChanged:dceplayer.setWindowSize(manager.appHeight, manager.appWidth)
            onMediaPlayerIdChanged:{
                 console.log("initializing media player")
                dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
            }
        }
        onCurrentStatusChanged:logger.logMediaMessage(dceplayer.currentStatus)
        onMediaBufferChanged: console.log("media buffer change:"+mediaBuffer)
        onMediaPlayingChanged: console.log("Media Playback status changed locally "+dceplayer.mediaBuffer)
    }

    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)
    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width

    function checkLayout()
    {
        console.log("c++ slot orientation changed")
    }

    DebugPanel{
        id:dcemessages
        debugMessage: dcemessage
        z:2
        anchors.top: parent.top
    }

    DebugPanel{
        id:mediaMessages
        debugMessage: manager.mediaResponse
        z:2
        anchors.top: dcemessages.bottom
    }

    DebugPanel{
        id:commandmessages
        debugMessage: manager.commandResponse
        z:2
        anchors.top: mediaMessages.bottom
    }

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

    ScreenSaver
    {   id:ss
        height: manager.appHeight
        width: manager.appWidth
        anchors.centerIn: parent
        z:-1
        MouseArea{
            anchors.fill: ss
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onClicked:  Qt.RightButton ? pageLoader.visible = !pageLoader.visible: ""
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
    function screenchange(screenname )
    {
        pageLoader.source = "screens/"+screenname
        if (pageLoader.status == Component.Ready)
        {
            var s = String(screenname)
            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
        }
        else if (pageLoader.status == Component.Loading)
        {
           logger.userLogMsg = "loading page from network"
            finishLoading(screenname)
        }
        else
        {
            logger.userLogMsg ="Command to change to:" + screenname + " failed!"
            screenfile = screenname
            pageLoader.source = "screens/Screen_x.qml"
        }
    }

    function finishLoading (screenname)
    {
        if(pageLoader.status != Component.Ready)
        {
           logger.userLogMsg="finishing load";
            pageLoader.source = "screens/"+screenname
           logger.userLogMsg = "screen" + screenname + " loaded."

        }
        else
        {
            finishLoading(screenname)
        }

    }

    function checkStatus(component)
    {
      logger.userLogMsg = "Checking progress:"+(component.progress)
    }


    Loader {
        id:pageLoader
        objectName: "loadbot"

        onSourceChanged:  loadin
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
        z:5

    }
    //=================Components==================================================//
    function loadComponent(componentName )
    {
        componentLoader.source = "components/"+componentName
        if (componentLoader.status == Component.Ready)
        {
           logger.userLogMsg ="Command to change to:" + componentName+ " was successfull"
        }
        else if (componentLoader.status == Component.Loading)
        {
            logger.userLogMsg = "loading page from network"
            finishLoadingComponent(componentName)
        }
        else
        {
            logger.userLogMsg = "Command to add: " + componentName + " failed!"

        }
    }

    function finishLoadingComponent (componentName)
    {
        if(componentLoader.status != Component.Ready)
        {
            logger.userLogMsg = "finishing network load"
            componentLoader.source = "components/"+componentName
          logger.userLogMsg="screen" + componentName + " loaded."
        }
        else
        {
            finishLoadingComponent(componentName)
        }

    }


    Loader{
        id:componentLoader
        height: parent.height
        width: parent.width
        objectName: "componentbot"
        onLoaded: {console.log("Component is loaded")
            componentLoader.z = 5
        }
        z:5
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
