/*!
 *\class Main main.qml
 *\brief The main logic component
 *
 *\ingroup desktop_noir
 *
 *This is a common component used to implement consistent persistent logic.
 */

import QtQuick 1.1
import Qt.labs.shaders 1.0
import AudioVisual 1.0
import DceScreenSaver 1.0
import "../../skins-common/lib/components"
import "../../skins-common/lib/handlers"
import "components"

Item {
    id: qmlroot
    width:manager.appWidth
    height:manager.appHeight
    //focus:true
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)
    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width
    onActiveFocusChanged: {
        pageLoader.forceActiveFocus()
    }
    Rectangle{
        id:filler
        anchors.fill: qmlroot
        color: "black"
    }

    Connections{
        target: manager
        onOrientationChanged: checkLayout()
    }

    Connections{
        target: dcenowplaying
        onMediaStatusChanged:{checkPlayStatus(); console.log("Media Playback changed to ==>"+dcenowplaying.b_mediaPlaying)}
    }

    function checkPlayStatus(){
        if(dcenowplaying.b_mediaPlaying && manager.currentScreen==="Screen_1.qml")
        {
            console.log("going to screen "+ dcenowplaying.qs_screen)
            manager.setCurrentScreen(dcenowplaying.qs_screen)
        }
    }

    Component.onCompleted: {
        dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
    }


    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../skins-common/fonts/Sawasdee.ttf"
    }

    ListModel{
        id:scenarios
        ListElement{
            name:"Lights"
            modelName:"currentRoomLights"
        }
        ListElement{
            name:"Media"
            modelName:"currentRoomMedia"
        }
        ListElement{
            name:"Climate"
            modelName:"currentRoomClimate"
        }
        ListElement{
            name:"Telecom"
            modelName:"currentRoomTelecom"
        }
        ListElement{
            name:"Security"
            modelName:"currentRoomSecurity"
        }
        ListElement{
            name:"Advanced"
            modelName:"advancedMenu"
        }
    }

    ListModel{
        id:advancedMenu

        ListElement{
            title:"KDE Desktop"
            params:0
        }

        ListElement{
            title:"Quick Reload"
            params:1
        }
        ListElement{
            title:"Regenerate"
            params:2
        }
        ListElement{
            title:"Power"
            params:3
        }
        ListElement{
            title:"Advanced"
            params:5
        }
        ListElement{
            title:"Computing"
            params:6
        }
        ListElement{
            title:"Exit"
            params:7
        }

    }

    ListModel{
        id:media_filters
        ListElement{
            name:"Attribute"
            pksort:10
        }
        ListElement{
            name:"Genre"
            pksort:4
        }
        ListElement{
            name:"MediaType"
            pksort:7

        }
        ListElement{
            name:"Resolution"
            pksort:-1
        }
        ListElement{
            name:"Source"
            pksort:5
        }
        ListElement{
            name:"View"
            pksort:-1
        }
    }

    Loader{
        id:overLay
        source: ""
        onSourceChanged: {
            if(source !== ""){
                z=10
                overLay.forceActiveFocus()
            }
            else
            {
                z=0

            }

            if(overLay.status === Component.Ready){
                overLay.forceActiveFocus()
            }
            else{
                pageLoader.forceActiveFocus()
            }
        }
    }

    function checkLayout()
    {
        console.log("c++ slot orientation changed")
        console.log(manager.appHeight+" x " + manager.appWidth)
        pageLoader.forceActiveFocus()
    }


    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }


    function screenchange(screenname )
    {
        pageLoader.source = "screens/"+screenname
        if (pageLoader.status == Component.Ready)
        {
            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
        }
        else if (pageLoader.status == Component.Loading)
        {
            console.log("loading page from network")
            finishLoading(screenname)
        }
        else
        {
            console.log("Command to change to:" + screenname + " failed!")
            screenfile = screenname
            pageLoader.source = "screens/Screen_x.qml"
        }
    }

    function finishLoading (screenname)
    {
        if(pageLoader.status != Component.Ready)
        {
            console.log("finishing load")
            pageLoader.source = "screens/"+screenname
            console.log("screen" + screenname + " loaded.")
        }
        else
        {
            finishLoading(screenname)
        }

    }

    function checkStatus(component)
    {
        console.log(component.progress)
    }
    //=================Components==================================================//
    function loadComponent(componentName )
    {
        componentLoader.source = "components/"+componentName
        if (componentLoader.status === Component.Ready)
        {
            manager.setDceResponse("Command to change to:" + componentName+ " was successfull")
        }
        else if (componentLoader.status === Component.Loading)
        {
            console.log("loading page from network")
            finishLoadingComponent(componentName)
        }
        else
        {
            console.log("Command to add: " + componentName + " failed!")

        }
    }

    function finishLoadingComponent (componentName)
    {
        if(componentLoader.status !== Component.Ready)
        {
            console.log("finishing network load")
            componentLoader.source = "components/"+componentName
            console.log("screen" + componentName + " loaded.")
        }
        else
        {
            finishLoadingComponent(componentName)
        }

    }

    function swapFocus(){

        console.log("Swap Focus Function.")

        if(hdr.activeFocus)
        { console.log("Header had focus, set to page loader.")
            pageLoader.forceActiveFocus()
        }
        else if (pageLoader.activeFocus)
        {
            console.log("Pageloader had, sending to footer menu.")
            ftr.forceActiveFocus()
        }
        else if(ftr.activeFocus)
        {
            console.log("Footer Had focus, sending to video plane.")
            dceplayer.forceActiveFocus()
        }
        else if(dceplayer.activeFocus)
        {   console.log("Player had focus, sending to header.")
            hdr.forceActiveFocus()
        }
        else
            pageLoader.forceActiveFocus()


        console.log("Header Focus::"+hdr.activeFocus)
        console.log("Loader Focus::"+pageLoader.activeFocus)
        console.log("Footer Focus::"+ftr.activeFocus)
        console.log("Dceplayer Focus::"+dceplayer.activeFocus)

        ftr.currentItem = -1
    }


    DceScreenSaver{
        id:glScreenSaver
        height: manager.appHeight
        width: manager.appWidth
        focus:true
        interval:8000
        anchors.centerIn: parent
        requestUrl:manager.currentRouter
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

    MediaManager{
        id:dceplayer
        anchors.top: parent.top
        anchors.left:parent.left

        onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
        onActiveFocusChanged: {
            if(activeFocus){
                console.log("Media Player has focus")
                pageLoader.forceActiveFocus()
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
                dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
            }
        }

        onCurrentStatusChanged:logger.logMediaMessage("Media Player Status::"+dceplayer.currentStatus)
        onMediaBufferChanged: console.log("media buffer change:"+mediaBuffer)
        onMediaPlayingChanged: console.log("Media Playback status changed locally "+dceplayer.mediaBuffer)
        onVolumeChanged:console.log(volume)
        Keys.onVolumeDownPressed: manager.adjustVolume("-1")
        Keys.onVolumeUpPressed:  manager.adjustVolume("+1")
        Keys.onTabPressed: ftr.forceActiveFocus()

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

    Item{
        id:vIndicator
        height: volLevel.paintedHeight
        width: volLevel.paintedWidth
        opacity:0
        Timer{
            id:fader
            running:false
            interval: 1100
            onTriggered: vIndicator.opacity=0
        }


        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: .65
        }

        StyledText{
            id:volLevel
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            text:dceplayer.volume
            color: "white"
            fontSize: 42
            font.bold: true
            onTextChanged: {
                fader.restart()
                vIndicator.opacity=1
            }
        }

        Behavior on opacity {
            PropertyAnimation{
                duration: 750
            }
        }
    }



    Loader {
        id:pageLoader
        objectName: "loadbot"
        onSourceChanged:  loadin
        focus:true
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
            pageLoader.forceActiveFocus()

        }
        onActiveFocusChanged: {
            if(activeFocus)
            {  console.log("Pageloader gained active focus");   }
            else
            { console.log("Page loader lost active focus");   }
        }
        Keys.onTabPressed:{
            console.log("Tab Focus Swap.")
            ftr.forceActiveFocus()
            pageLoader.focus = false;
        }
        Keys.onPressed: {
            if(event.key === Qt.Key_M){
                console.log("Show Menu")
            } else if (event.key === Qt.Key_H){
                console.log("Show Header")
            }
        }
    }



    STBHeader {
        id: hdr

    }
    STBFooter {
        id: ftr
    }

    Style{
        id:appStyle
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


