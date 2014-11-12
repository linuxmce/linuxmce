import QtQuick 2.0
import DceScreenSaver 1.0

import "../../../../skins-common/lib/handlers"
import "components"

Item {
    id: qml_root
    onWidthChanged: console.log(manager.appWidth+"::"+manager.appHeight)
    height: manager.appWidth
    width: manager.appHeight

    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)
    signal showUi(bool uiState)
    property variant current_scenario_model
    property variant current_header_model:scenarios
    property variant audioMediaTypes:[4]
    property variant videoMediaTypes:[5,11]

    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width
    property bool showingUi:true

    function scaleX(x){
        return x/100*qml_root.width
    }
    function scaleY(y){
        return y/100*qml_root.height
    }

    Style{
        id:style
    }

    Component.onCompleted: {state="wide"; }

    focus:true
    Keys.onReleased:{

        switch(event.key){
        case Qt.Key_Menu:
            console.log("menu button caught in root!")
            break;
        case Qt.Key_VolumeUp:
            console.log("Vol up")
            break;
        case Qt.Key_VolumeDown:
            console.log("vol down")
            break;
        case Qt.Key_MediaPrevious:
            console.log("Caught back button! Phew!")
            manager.goBacktoQScreen()
            break;
        case Qt.Key_Back:
            console.log("Caught Back again! Tricky...")
            manager.goBacktoQScreen()
            break;
        default:
            console.log("I have no idea what key " + event.key + " is. ")
            break;
        }

        event.accepted= true
    }


    DceScreenSaver{
        id:glScreenSaver
        anchors{
            top:qml_root.top
            bottom:qml_root.bottom
            left: qml_root.left
            right: qml_root.right
        }
        //        height:manager.appHeight
        //        width: manager.appWidth
        interval:60000

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
            onClicked: {
                if(manager.currentScreen!=="Screen_1.qml" && !showingUi){
                    toggleUi(!showingUi)
                } else if (manager.currentScreen==="Screen_1.qml"){
                    toggleUi(!showingUi)
                }
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

    //    Timer{
    //        id:mini_ss_timer
    //        interval:10000
    //        running: false // screensaver.active
    //        triggeredOnStart: true
    //        onTriggered:mini_screen_saver_image.source= "http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=screensaver&val="+manager.getNextScreenSaverImage(mini_screen_saver_image.source)
    //        repeat: true
    //    }
    //    Item{
    //        id:mini_screen_saver
    //        anchors{
    //            top:parent.top
    //            left:parent.left
    //            right:parent.right
    //            bottom:parent.bottom
    //        }

    //        Component.onCompleted: {
    //            bgFill.color="blue"
    //            mini_ss_timer.running = true
    //            mini_ss_timer.start()
    //            console.log("Orbiter Consume Screensaver images")
    //        }



    //        Image{
    //            id:mini_screen_saver_image
    //            height: mini_screen_saver.height
    //            width: mini_screen_saver.width
    //            source:"http://"+manager.m_ipAddress+"/lmce-admin/imdbImage.php?type=screensaver&val="+manager.getNextScreenSaverImage(source)

    //        }
    //    }


    Rectangle{
        anchors.fill: nav_row
        color:manager.connectedState ? "black" : "red"
        opacity: .65

        Behavior on color {
            ColorAnimation {  duration: style.animation_quick }
        }
    }

    NavigationRow {
        id: nav_row
    }
    MediaPopup{
        id:media_notification

    }
    InformationPanel {
        id: info_panel
    }
    Loader {
        id:pageLoader
        objectName: "loadbot"
        focus: true
        state:"active"
        clip:true
        Connections{
            target:qml_root
            onShowUi:{
                if(uiState){
                    state="active"
                }else{
                    state="hidden"
                }
            }
        }

        anchors{
            top: manager.currentScreen==="Screen_1.qml" ? media_notification.bottom : nav_row.bottom
            bottom:info_panel.top
            left:showingUi ? parent.left : parent.right
            right:parent.right
        }
        Keys.onBackPressed: console.log("back")
        onSourceChanged:  loadin
        onLoaded: {

            console.log("Screen Changed:" + pageLoader.source)
        }
        onStatusChanged:  if (pageLoader.status == Component.Ready)
                          {
                              manager.setDceResponse("Command to change to:" + source+ " was successfull")
                          }
                          else if (pageLoader.status == Component.Loading)
                          {
                              console.log("loading page from network")

                          }
                          else if(pageLoader.status == Component.Error)
                          {
                              console.log("Command to change to:" + source + " failed!")

                              pageLoader.source = "screens/Screen_X.qml"
                              screenfile = source

                          }

        states: [
            State {
                name: "hidden"
                PropertyChanges {
                    target: pageLoader
                    visible:false
                    enabled:false

                }
            },
            State {
                name: "active"
                PropertyChanges {
                    target: pageLoader
                    visible:true
                    enabled:true
                }
            }
        ]
        transitions: [
            Transition {
                from: "*"
                to: "*"
                PropertyAnimation{
                    duration: 500
                }
            }
        ]
    }

    MediaInterface{
        anchors.fill: parent
        id:mediaPlayer
    }


    function toggleBars(barState){
        if(barState){
            nav_row.state="hidden"
            info_panel.state="hidden"
        }else {
            nav_row.state="active"
            info_panel.state="retracted"
        }

    }

    function updateBackground(portait, wide){
        appBackground.pSource = portait
        appBackground.wSource = wide
    }


    function screenchange(screenname ){
        pageLoader.source = "screens/"+screenname
    }

    function checkStatus(component){
        console.log(component.progress)
    }

    FontLoader{
        id:appFont
        source:"../../../../skins-common/fonts/Sawasdee.ttf"
    }

    //=================Components==================================================//
    function loadComponent(componentName){
        componentLoader.source = "components/"+componentName
    }

    /* * */
    function toggleUi(toggleState){
        showUi(toggleState)
        showingUi = toggleState
    }

    function lowerInfoPanel(){
        info_panel.state="hidden"
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

    Loader{
        id:componentLoader
        anchors{
            top:nav_row.bottom
            bottom:info_panel.top
            left:parent.left
            right:parent.right
        }
        function close(){
            componentLoader.source=""
        }

        objectName: "componentbot"
        onStatusChanged: {
            console.log("Current Component Loader status is " + componentLoader.status)
        }

        onLoaded: {
            console.log("Component is loaded")
        }

        states: [
            State {
                name: "active"
                PropertyChanges {
                    target: componentLoader
                    enabled: true
                    opacity:1

                }
            },
            State {
                name: "hidden"
                PropertyChanges {
                    target: componentLoader
                    enabled: false
                    opacity:0
                }
            }
        ]
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

    states: [
        State {
            name: "wide"
            PropertyChanges {
                target: qml_root
                height:manager.appHeight
                width:manager.appWidth
                rotation:0
            }

        },
        State {
            name: "portrait"
            PropertyChanges {
                target: qml_root
                height:manager.appWidth
                width:manager.appHeight
                transformOrigin:qml_root.Center
                rotation:270
                x:0
                y:0
            }

        }

    ]
}
