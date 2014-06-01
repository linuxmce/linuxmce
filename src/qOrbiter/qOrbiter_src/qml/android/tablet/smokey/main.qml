import QtQuick 1.1
import DceScreenSaver 1.0
import AudioVisual 1.0
import "../../../skins-common/lib/handlers"
import "components"
//Tablet - Smokey

Item {
    id: qml_root
    height:manager.appHeight
    width:manager.appWidth

    property int screensaverTimer:15000 // manager.screenSaverTimeout*1000
    property bool uiOn:true
    //    anchors{
    //        top:parent.top
    //        bottom: parent.bottom
    //        left: parent.left
    //        right:parent.right
    //    }

    Timer{
        id:hideUiTimer
        interval:screensaverTimer
        running: false
        repeat: false
        onTriggered: {}
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

    Timer{
        id:refresh
        interval: 500
        running:true
        repeat: true
        onTriggered: {
            if(canary.rotation===360)
                canary.rotation =1
            else
                canary.rotation=(canary.rotation+1)
        }
    }

    MediaManager{
        id:dceplayer
        anchors.top: parent.top
        anchors.left:parent.left
        focus:true
        onMediaPlayingChanged: {
            if(!mediaPlaying)
                androidSystem.stopMedia()
        }


        onAndroidUrlUpdated:{
            console.log("NEW ANDROID URL")
            if(androidUrl.length > 4){
                console.log("URL ok!")
                androidSystem.playMedia(dceplayer.androidUrl)
            }
            else{
                console.log("Url Malformed!")
                console.log("Url::"+androidUrl)
            }
        }

        //  onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)


        Component.onCompleted: {
            if(manager.mediaPlayerID !== -1){
                dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                androidSystem.startAudioService(dceplayer.callbackAddress);
                console.log("initializing qml media player::"+manager.mediaPlayerID)

            }
        }

        Connections{
            target:manager
            onMediaPlayerIdChanged:{
                console.log("initializing qml media player::"+manager.mediaPlayerID)
                dcePlayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)

            }
        }

    }

    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    property variant current_scenario_model
    property variant current_header_model:scenarios

    property string locationinfo: "standby"
    property string screenfile

    function scaleX(x){
        return x/100*qml_root.width
    }
    function scaleY(y){
        return y/100*qml_root.height
    }

    function dumpObj(obj, message){
        console.log((message? message+"\n\t" : "No Message \n\t")+ JSON.stringify(obj, null, "\t"))
    }

    focus:true
    Keys.onReleased:{

        switch(event.key){
        case Qt.Key_Menu:
            console.log("menu button caught in root!")
            break;
        case Qt.Key_VolumeUp:
            console.log("Vol up")
            manager.adjustVolume(5)
            break;
        case Qt.Key_VolumeDown:
            console.log("vol down")
            manager.adjustVolume(-5)
            break;
        case Qt.Key_MediaPrevious:
            console.log("Caught back button! Phew!")
            break;
        case Qt.Key_Back:
            console.log("Caught Back again! Tricky...")
            break;
        default:
            console.log("I have no idea what key " + event.key + " is. ")
            break;
        }

        event.accepted= true
    }
    Rectangle{
        anchors.fill: parent
        id:bgFill
        color:"black"
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

    DceScreenSaver{
        id:glScreenSaver
        anchors{
            top:qml_root.top
            bottom:qml_root.bottom
            left: qml_root.left
            right: qml_root.right
        }
        interval:30000
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
            onClicked: {
                if(!uiOn){
                    console.log("screensaver revive")

                }
            }
        }

    }


    Loader {
        id:pageLoader
        objectName: "loadbot"
        focus: true
        visible:qml_root.uiOn
        anchors{
            top: nav_row.bottom
            bottom:info_panel.top
            left:qml_root.left
            right:qml_root.right
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


    function screenchange(screenname )
    {
        pageLoader.source = "screens/"+screenname

    }



    function checkStatus(component)
    {
        console.log(component.progress)
    }

    FontLoader{
        id:appFont
        source:"./../../../skins-common/fonts/Sawasdee.ttf"
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
            componentLoader.source = "components/"+comp
            console.log("item " + comp + " loaded.")
        }
        else
        {
            finishLoadingComponent(comp)
        }
    }

    GenericAttributeSelector{
        id:attributeSelector
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
}
