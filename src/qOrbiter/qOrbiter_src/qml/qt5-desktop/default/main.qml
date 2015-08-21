/*!
 *\class Main main.qml
 *\brief The main logic component
 *
 *\ingroup desktop_noir
 *
 *This is a common component used to implement consistent persistent logic.
 */

import QtQuick 2.1
import AudioVisual 1.0
import DceScreenSaver 1.0
import org.linuxmce.enums 1.0

import "components"

Item {
    id: qmlroot
    width:manager.appWidth
    height:manager.appHeight
    //focus:true
    signal close()
    signal showMetaData()
    signal changeScreen(string s)
    signal setupStart(int x, string y)
    property alias appStyle:skinStyle
    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width
    property bool uiOn:true
    property bool screenSaverOn:false
    property int screensaverTimer:manager.screenSaverTimeout*1000

    property string currentorbiterWindow:screenInfo.screenName

    function resetUiTimeout(){
        hideUiTimer.restart()
        screenSaverOn=false
    }
Style{
id:skinStyle
}
    onActiveFocusChanged: {
        //  uiOn=true
    }

    Component.onCompleted: {
        dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
    }

    Connections{
        target: manager
        onOrientationChanged: checkLayout()
        onDceRemoteCommand:{
            resetUiTimeout()


            if(manager.currentScreen=="Screen_1.qml" && !ftr.isActive){
                uiOn=!uiOn
                console.log("Main.qml::command #"+cmd + " & name "+name+"is unhandled")
                if(!uiOn){
                    uiOn=true;
                    return;
                }
            }
        }
        onScreenSaverImagesReady:glScreenSaver.setImageList(manager.screensaverImages)
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

    function showMenu(yay){
        if(yay){
            pageLoader.focus=false
        } else {
            pageLoader.forceActiveFocus()
        }

    }


    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }


    function swapFocus(){
        uiOn =!uiOn
        console.log("Swap Focus Function.")
        if(uiOn){
            ftr.forceActiveFocus()
        } else {
            pageLoader.forceActiveFocus()
        }

        console.log("Header Focus::"+hdr.activeFocus)
        console.log("Loader Focus::"+pageLoader.isActive)
        console.log("Footer Focus::"+ftr.isActive)
        console.log("Dceplayer Focus::"+dceplayer.activeFocus)

        if(dceplayer.mediaPlaying && pageLoader.activeFocus){
            dceplayer.forceActiveFocus()
        }

    }



    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../skins-common/fonts/Sawasdee.ttf"
    }

    Timer{
        id:hideUiTimer
        interval:screensaverTimer
        running: false
        repeat: true
        onTriggered: {
            uiOn=false
            screenSaverOn=true
        }
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
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
            bottom:parent.bottom
        }

        onSourceChanged: {
            if(source !== ""){
                z=10
                overLay.forceActiveFocus()
            } else {
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



    DceScreenSaver{
        id:glScreenSaver
        anchors{
            top:qmlroot.top
            bottom:qmlroot.bottom
            left: qmlroot.left
            right: qmlroot.right
        }
        enableDebug: true
        interval:10000
        useAnimation: true
        onDebugInfoChanged: console.log(debugInfo)
        active:true //manager.m_ipAddress==="192.168.80.1"
        requestUrl:manager.m_ipAddress
        Component.onCompleted: {
            glScreenSaver.setImageList(manager.screensaverImages)
            console.log("Orbiter Consume Screensaver images")
            console.log("Orbiter counts " + glScreenSaver.pictureCount)
        }

        Connections{
            target:manager
            onScreenSaverImagesReady:{
                glScreenSaver.setImageList(manager.screensaverImages)
                console.log("Orbiter Consume Screensaver images")
                console.log("Orbiter counts " + glScreenSaver.pictureCount)
            }
        }

//        MouseArea{
//            anchors.fill: parent
//            onClicked: {
//                if(!uiOn){
//                    console.log("screensaver revive")
//                    uiOn=true
//                }
//            }
//        }

    }

    DceMedia{
        id:dceplayer
        anchors.top: dceplayer.videoStream ? parent.top : parent.bottom
        anchors.left:parent.left

        focus:true
        // onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
        onActiveFocusChanged: {
            if(activeFocus){
                console.log("Media Player has focus")
                //  pageLoader.forceActiveFocus()
            }
        }

        Component.onCompleted: {
            setorbiterWindowSize(manager.appHeight, manager.appWidth);
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

    STBHeader {
        id: hdr
    }

    function screenchange(screenname ){
        if(pageLoader.currentScreen!=screenname)
            pageLoader.nextScreen = screenname
    }

    function checkStatus(component){
        console.log(component.progress)
    }

    Loader {
        id:pageLoader
        objectName: "loadbot"

        property bool isActive:false
        property string nextScreen:"Screen_1.qml"
        property string currentScreen:""

        onNextScreenChanged: {

            if(!nextScreen.match(".qml")){
                return
            } else {

            }

            if(!pageLoader.item){
                console.log("Last screen likely had errors, loading next screen==>"+nextScreen)
                loadNext()
            }

            if( pageLoader.item.noForce===true){
                console.log("pageloader::"+source+"\n\tdeclares noforce::"+pageLoader.item.noForce+",\n\tignoring "+nextScreen)
                return;
            }else {
                console.log("pageloader::"+source+"\n\tnoforce::"+pageLoader.item.noForce+"\n\tloading next screen.")
                console.log("next screen==>"+nextScreen)
                startChange()
            }

        }

        function startChange(){

            if(pageLoader.item && pageLoader.item.screen){
                console.log("pageloader::closing page "+ pageLoader.item.screen)
                pageLoader.item.state="closing"
            } else{
                console.log("pageloader::no page jumping to next ==>"+nextScreen)
                loadNext()
            }

        }

        function loadNext(){


            if(nextScreen===""){
                nextScreen="Screen_1.qml"
                return
            }

            console.log("pageloader::loading next screen screens/"+nextScreen)
            pageLoader.source="screens/"+nextScreen
        }

        source: "screens/Screen_1.qml"
        focus:true
        anchors{
            top:hdr.bottom
            left:qmlroot.left
            bottom:ftr.top
            right:qmlroot.right
        }
      //  opacity: !screenSaverOn ? 1 : 0
        Behavior on opacity {
            PropertyAnimation{
                duration: 750
            }
        }
        onStatusChanged: {
            switch(pageLoader.status){
            case Component.Ready:
                manager.setDceResponse("Command to change to:" + source+ " was successfull")
                screenfile = source
                currentScreen=nextScreen

               // pageLoader.item.forceActiveFocus();

//                if(currentScreen==="Screen_1.qml")
//                    uiOn=true
//                else
//                    uiOn=false
                // contentItem=item.screen_root
                break;
            case Component.Loading:
                console.log("loading page from network")
                break;
            case Component.Error:
                console.log("Command to change to:" + source + " failed!")
                manager.currentScreen="Screen_x.qml"
                break;
            default:
                logger.logSkinMessage("Unhandled loader state. please fix asap::State::"+Component.status)
                break;
            }
        }

        onActiveFocusChanged: {
            if(activeFocus){
                isActive=true
                console.log("Pageloader gained active focus");
                item.forceActiveFocus()
            }
            else if (pageLoader.item.activeFocus){
                console.log("Pageloader content gained active focus");
            }
            else{
                isActive=false
                console.log("Page loader lost active focus ::"+ pageLoader.activeFocus);
            }
        }
        onIsActiveChanged: console.log("New pageloader content focus is "+isActive)

        Keys.onTabPressed:{
            swapFocus()
        }
        Keys.onPressed: {
            if(event.key === Qt.Key_M){
                console.log("Show Menu")
            } else if (event.key === Qt.Key_H){
                console.log("Show Header")
            }
        }
    }

    STBFooter {
        id: ftr
    }


    MouseArea{
        id:mst
        anchors.fill: parent
        acceptedButtons: Qt.RightButton | Qt.LeftButton | Qt.MidButton
        onPressed: {
            if(mouse.button==Qt.RightButton){
                console.log("Event Filter swapping focus")
                swapFocus()
                mouse.accepted=true
                return;
            }
            uiOn=true
            mouse.accepted=false
            console.log("Mouse X: "+mouse.x)
            console.log("Mouse Y:"+mouse.y)
            console.log("\n")
            hideUiTimer.restart()
        }
    }
}


