import QtQuick 2.2
import DceScreenSaver 1.0
import AudioVisual 1.0

import "../../../../skins-common/qt5/default/components"
//Tablet - Smokey

Item {
    id: qml_root
    height:manager.appHeight
    width: manager.appWidth

    property alias skinStyle:style
    property int screensaverTimer:manager.screenSaverTimeout*1000
    property bool uiOn:true

    property variant current_scenario_model:currentRoomLights
    property variant current_header_model:scenarios
    property string locationinfo: "standby"
    property string screenfile:""

    signal showMetadata()
    signal showPlaylist()
    signal showAdvanced()
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    function scaleX(x){
        return x/100*qml_root.width
    }
    function scaleY(y){
        return y/100*qml_root.height
    }

    function dumpObj(obj, message){
        console.log((message? message+"\n\t" : "No Message \n\t")+ JSON.stringify(obj, null, "\t"))
    }

    function raiseNavigation(raised){
        if(raised ){
            nav_row.state="raised"
        } else {
            nav_row.state="lowered"
        }
    }


    focus:true

    Rectangle{
        anchors.fill: parent
        id:bgFill
        color:"black"
    }

    onUiOnChanged: {
        console.log("UiOn::"+uiOn)
        if(uiOn){
            if(info_panel.restore){
                info_panel.state="retracted"
                info_panel.restore=false
            }
        } else {
            if(info_panel.state==="retracted"){
                info_panel.restore=true
            } else {
                info_panel.restore=false
            }

            info_panel.state="hidden"
        }
    }


    Timer{
        id:hideUiTimer
        interval:screensaverTimer
        running: true
        repeat: true
        onTriggered: {
            if(uiOn){
                uiOn=false
                if(glScreenSaver.active){
                    glScreenSaver.forceActiveFocus()
                } else {
                    mobileGradient.forceActiveFocus()
                }
            }
        }
    }

    function screenSaverMode(running){
        if(running){

        } else {

        }
    }

    Rectangle{
        id:canary
        height: 1
        width: 1
        color:"white"
    }
    Style{
        id:style
    }

    //    Timer{
    //        id:refresh
    //        interval: 500
    //        running:true
    //        repeat: true
    //        onTriggered: {
    //            if(canary.rotation===360)
    //                canary.rotation =1
    //            else
    //                canary.rotation=(canary.rotation+1)
    //        }
    //    }


    Keys.onReleased:{
        switch(event.key){

        case Qt.Key_Menu:
            showOptions=!showOptions
            console.log("menu button caught in root")
            break;

        case Qt.Key_Back:
            if(manager.currentScreen==="Screen_1.qml"){
                showExitConfirm()
            } else{
                manager.goBacktoQScreen()
            }
            console.log("Caught Back Key")
            break
        case Qt.Key_MediaPrevious:
            if(manager.currentScreen==="Screen_1.qml"){
                showExitConfirm()
            } else{
                manager.goBacktoQScreen()
            }
            break;
        case Qt.Key_VolumeDown:
            androidSystem.mediaSetVol(-1)
            console.log("vol-")
            break;
        case Qt.Key_VolumeUp:
            androidSystem.mediaSetVol(+1)
            console.log("vol+")
            break;

        default:
            console.log(event.key)
        }
        event.accepted=true
    }


    ListModel{
        id:scenarios
        ListElement{
            name:"Lights"
            modelName:"currentRoomLights"
            floorplantype:2
        }
        ListElement{
            name:"Media"
            modelName:"currentRoomMedia"
            floorplantype:5
        }
        ListElement{
            name:"Climate"
            modelName:"currentRoomClimate"
            floorplantype:3
        }
        ListElement{
            name:"Telecom"
            modelName:"currentRoomTelecom"
            floorplantype:6
        }
        ListElement{
            name:"Security"
            modelName:"currentRoomSecurity"
            floorplantype:4
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



    property color mobilegradientone:"darkgreen"
    property color mobilegradienttwo:style.contentBgColor
    property variant colorArray:["darkblue", "black", "grey", "darkcyan" ,"purple", "slategrey", "crimson", "gold", "dodgerblue"]

    Timer{
        id:colorTimer
        interval: 5000
        onTriggered: {
            mobilegradientone = colorArray[Math.floor(Math.random() * colorArray.length)]
            mobilegradienttwo = colorArray[Math.floor(Math.random() * colorArray.length)]
        }
        running:mobileGradient.visible
        repeat: true
    }

    Rectangle{
        id:mobileGradient
        anchors.fill: parent
        visible:!glScreenSaver.active
        gradient:Gradient{
            GradientStop{
                color: mobilegradientone
                position: 0.0
                Behavior on color {
                    ColorAnimation {  duration: 2000 }
                }

            }

            GradientStop{
                color:mobilegradienttwo
                position: .65
                Behavior on color {
                    ColorAnimation {  duration: 2000 }
                }
            }
        }
        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: .50
            Behavior on color {
                ColorAnimation {  duration: 2000 }
            }

        }
        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(!uiOn){
                    console.log("screensaver revive")
                    uiOn=true
                }
            }
        }
    }
    DceScreenSaver{
            id:glScreenSaver
           anchors{
                top:qml_root.top
                bottom:qml_root.bottom
                left: qml_root.left
                right: qml_root.right
            }
            enableDebug: true
            interval:60*1000
            useAnimation: true
            //onDebugInfoChanged: console.log(debugInfo)
           active:manager.connectedState
            requestUrl:manager.currentRouter
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

            MouseArea{
                anchors.fill: parent
                onClicked: {
                   if(!uiOn){
                        console.log("screensaver revive")
                        uiOn=true
                    }
                }
            }

        }
    MediaInterface{
     id:dcePlayer
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
        focus: true
        source: style.commonQmlPath+"screens/Screen_1.qml"
        // visible:qml_root.uiOn
        property string nextScreen:"Screen_1.qml"
        property string currentScreen:""
        anchors{
            top: nav_row.bottom
            bottom:info_panel.top
            left:qml_root.left
            right:qml_root.right

        }

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
                console.log("pageloader::"+source+" declares noforce::"+pageLoader.item.noForce+", ignoring "+nextScreen)
                return;
            }else {
                console.log("pageloader::"+source+" noforce::"+pageLoader.item.noForce+" , loading next screen.")
                console.log("next screen==>"+nextScreen)
                startChange()
            }

        }

        function startChange(){


            if(!pageLoader.item || pageLoader.item.scree){
                console.log("pageloader::closing page "+ manager.currentScreen)
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

            console.log("pageloader::loading next screen::\n"+style.commonQmlPath+"screens/"+nextScreen)
            pageLoader.source=style.commonQmlPath+"screens/"+nextScreen
        }

        opacity: uiOn ? 1 : 0
        Behavior on opacity {
            PropertyAnimation{
                duration: 750
            }
        }

        Keys.onBackPressed: console.log("back")

        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
        onStatusChanged:  if (pageLoader.status == Component.Ready)
                          {

                              manager.setDceResponse("Command to change to:" + source+ " was successfull")
                              screenfile = source
                              currentScreen=nextScreen

                              // contentItem=item.screen_root
                          }
                          else if (pageLoader.status == Component.Loading)
                          {
                              console.log("loading page from network")

                          }
                          else if(pageLoader.status == Component.Error)
                          {
                           // pageLoader.e
                              console.log("Command to change to:" + source + " failed!")
                              manager.currentScreen="Screen_X.qml"
                          }
    }
    MediaPopup{
        id:media_notification
    }
    NavigationRow {
        id: nav_row
    }


    InformationPanel {
        id: info_panel
    }

    function hideInfoPanel(){
        info_panel.state = "hidden"
    }

    function showInfoPanel(){
        info_panel.state = "retracted"
    }

    function setNavigation(n){
        nav_row.navSource=""
        nav_row.navSource = n
    }

    function updateBackground(portait, wide){
        appBackground.pSource = portait
        appBackground.wSource = wide
    }




    FontLoader{
        id:appFont
        source:".././../../../skins-common/fonts/Sawasdee.ttf"
    }

    //=================Components==================================================//
    function loadComponent(componentName)
    {
        componentLoader.source = "components/"+componentName
        if (componentLoader.status == Component.Ready)
        {
            manager.setDceResponse("Command to load:" + componentName+ " was successfull")

        }
        else if (componentLoader.status == Component.Loading)
        {
            console.log("loading item from network")
            finishLoadingComponent(componentName)
        }
        else
        {
            console.log("Command to load: " + componentName + " failed!")
            componentFile = componentName
        }
    }

    function finishLoadingComponent (comp)
    {
        if(componentLoader.status != Component.Ready)
        {
            console.log("finishing network load")
            componentLoader.source = style.commonQmlPath+"components/"+comp
            console.log("item " + comp + " loaded.")
        }
        else
        {
            finishLoadingComponent(comp)
        }
    }



    Loader{
        id:componentLoader
        height: parent.height
        width: parent.width
        objectName: "componentbot"
        onLoaded: {
            console.log("Component is loaded")
        }
    }



    MouseArea{
        id:mst
        anchors.fill: parent

        onPressed: {
            mouse.accepted=false
            console.log("Mouse X: "+mouse.x)
            console.log("Mouse Y:"+mouse.y)
            console.log("\n")
            hideUiTimer.restart()
        }
    }
}
