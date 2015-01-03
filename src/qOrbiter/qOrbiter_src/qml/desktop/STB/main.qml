/*!
 *\class Main main.qml
 *\brief The main logic component
 *
 *\ingroup desktop_noir
 *
 *This is a common component used to implement consistent persistent logic.
 */

import QtQuick 1.1
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
    signal showMetaData()
    signal changeScreen(string s)
    signal setupStart(int x, string y)
    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width
    property bool uiOn:true
    property int screensaverTimer:15000 //manager.screenSaverTimeout*1000

    DceScreenSaver{
        id:glScreenSaver
        height: manager.appHeight
        width: manager.appWidth
        x:dcenowplaying.b_mediaPlaying ? parent.x + width :parent.x
        y:parent.y
        interval:30000

        Behavior on x{
            PropertyAnimation{
                duration: 350
                easing.type: appStyle.animationEasing
            }
        }

        useAnimation: true
        onDebugInfoChanged: console.log(debugInfo)
        active:true//manager.m_ipAddress==="192.168.80.1"
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


//        MouseArea{
//            anchors.fill: parent
//            hoverEnabled: true
//            onPressed:if(glScreenSaver.activeFocus) hideUI()
//        }

    }

    onActiveFocusChanged: {
        pageLoader.forceActiveFocus()
    }

    //    Component.onCompleted: {
    //        dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
    //    }

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
        }else if (pageLoader.activeFocus){
            console.log("Pageloader had, sending to footer menu.")
            ftr.forceActiveFocus()
        }else if(ftr.activeFocus){
            console.log("Footer Had focus, sending to video plane.")
            pageLoader.forceActiveFocus()
          //  dceplayer.forceActiveFocus()
        }else {
            pageLoader.forceActiveFocus()
        }

        console.log("Header Focus::"+hdr.activeFocus)
        console.log("Loader Focus::"+pageLoader.activeFocus)
        console.log("Footer Focus::"+ftr.activeFocus)


        ftr.currentItem = -1
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
        repeat: false
        onTriggered: {
            if(uiOn){
                uiOn=false
            } else {
                uiOn = true
                ftr.forceActiveFocus()
            }

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
           // texyer.volume
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

    Loader {
        id:pageLoader
        objectName: "loadbot"
        onSourceChanged:  loadin
        property bool isActive:item.activeFocus
        focus:true
        anchors{
            top:hdr.bottom
            left:qmlroot.left
            bottom:ftr.top
            right:qmlroot.right
        }

        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
            pageLoader.forceActiveFocus()
        }

        onActiveFocusChanged: {

            if(pageLoader.item ==null ){
                pageLoader.source=manager.currentScreen
            }

            if( activeFocus){
                manager.logSkinMessage("Pageloader gained active focus");
            } else if (pageLoader.item.activeFocus){
                manager.logSkinMessage("Pageloader content gained active focus");
            }
            else{
                manager.logSkinMessage("Page loader lost active focus ::"+ pageLoader.activeFocus);
            }
        }
        onIsActiveChanged: console.log("New pageloader content focus is "+isActive)

        Keys.onTabPressed:{
            console.log("Tab Focus Swap.")
            ftr.forceActiveFocus()
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
    MouseArea{
        id:mst
        anchors.fill: parent

        onPressed: {
            mouse.accepted=false
            console.log("Mouse X: "+mouse.x)
            console.log("Mouse Y:"+mouse.y)
            console.log("\n")
            //   hideUiTimer.restart()
        }
    }
}


