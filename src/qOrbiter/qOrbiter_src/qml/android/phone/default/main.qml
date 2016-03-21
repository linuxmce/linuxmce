import QtQuick 1.1
import "components"
import "js/ComponentLoader.js" as MyJs
import DceScreenSaver 1.0
import AudioVisual 1.0
//phone - default
Item {
    id: qmlroot
    width:manager.appWidth
    height:manager.appHeight
    focus:true
    property bool profile:!manager.isProfile
    property bool uiOn:true
    property alias skinStyle:appStyle
    property int screensaverTimer:manager.screenSaverTimeout*1000
    property string lastLoaderError:""

    signal showMetadata()
    signal showPlaylist()
    signal showAdvanced()

    Style{
        id:appStyle
    }

    Rectangle{
        id:canary
        height: 1
        width: 1
        color:"white"
    }

    FontLoader{
        id:appFont
        source: "../../../skins-common/fonts/scoutlight.ttf"
    }

    Timer{
        id:refresh
        interval: 750
        running:true
        repeat: true
        onTriggered: {
            if(canary.rotation===360)
                canary.rotation =1
            else
                canary.rotation=(canary.rotation+1)
        }
    }

    Timer{
        id:hideUiTimer
        interval:screensaverTimer
        running: true
        repeat: true
        onTriggered: {
            if(uiOn){
                console.log("starting screensaver.")
                uiOn=false
                if(glScreenSaver.active){
                    glScreenSaver.forceActiveFocus()
                } else {
                    mobileGradient.forceActiveFocus()
                }
            }
        }
    }

    Timer{
        id:tcCallback
        running:dceplayer.mediaPlaying
        interval: 1000
        repeat: true
        onTriggered: {console.log("tick");dceplayer.processTimeCode(androidSystem.getMediaPosition())}
    }

    MediaManager{
        id:dceplayer
        anchors.top: parent.top
        anchors.left:parent.left
        focus:true
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
        onTotalTimeChanged: {
            if(androidUrl.length==0){
                tcCallback.stop()
            } else {
                tcCallback.start()
            }
        }

        onMediaPlayingChanged: {
            if(!mediaPlaying){
                androidSystem.mediaStop()
                tcCallback.stop()
            } else {
                tcCallback.start()
            }

        }
        onAndroidVolumeUp:{
            androidSystem.mediaSetVol(1.0)
        }

        onAndroidVolumeDown:{
            androidSystem.mediaSetVol(-1.0)
        }

        onPauseMedia:{
            console.log("dceplayer::pause")
            androidSystem.mediaPause();
        }
        onUpdatePluginSeek:{
            console.log("dceplayer::seek to"+pos)
            androidSystem.mediaSeek(pos);
        }


        Component.onCompleted: {
            console.log("initializing qml media player::"+manager.mediaPlayerID)
            if(manager.mediaPlayerID !== -1){
                dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
                androidSystem.startAudioService(dceplayer.callbackAddress);
            }
        }

        Connections{
            target:manager
            onMediaPlayerIdChanged:{
                console.log("initializing qml media player::"+manager.mediaPlayerID)
                dcePlayer.setConnectionDetails(manager.mediaPlayerID, manager.currentRouter)
            }
            onConnectedStateChanged:{
                if(manager.connectedState ){
                    dceplayer.reInit();
                }
            }
        }

    }

    Component.onCompleted: {
        androidSystem.updateBuildInformation()
    }

    Keys.onReleased: {
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
            manager.adjustVolume(-1)
            console.log("vol-")
            break;
        case Qt.Key_VolumeUp:
            manager.adjustVolume(+1)
            console.log("vol+")
            break;

        default:
            console.log(event.key)
        }
        event.accepted=true
    }
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width
    property bool showOptions:false

    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }

    ListModel{
        id:scenarios

        function returncount(){
            return scenarios.count
        }

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
        ListElement{
            name:"Advanced"
            modelName:"advanced"
            floorplantype:-1
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


    Rectangle{
        id:mobileGradient
        anchors.fill: parent
        visible:!glScreenSaver.active
        gradient:Gradient{
            GradientStop{
                color: "darkgreen"
                position: 0.0
            }

            GradientStop{
                color:"black"
                position: .65
            }
        }
        Rectangle{
            anchors.fill: parent
            color:appStyle.toolbarBgColor

        }
    }

    DceScreenSaver{
        id:glScreenSaver
        height:qmlroot.height
        width: qmlroot.width
        interval:60000
        anchors.centerIn: qmlroot
        active:manager.currentRouter = manager.internalHost
        requestUrl:manager.currentRouter

        Component.onCompleted: {
            if(glScreenSaver.pictureCount===0 && glScreenSaver.active){
                glScreenSaver.setImageList(manager.screensaverImages)
            }
        }
    }



    Connections{
        target:manager
        onScreenSaverImagesReady:{
            console.log("Retrieving Screen Saver Images...")
            glScreenSaver.setImageList(manager.screensaverImages)
        }
    }


    function showExitConfirm(){

    }

    function checkStatus(component)
    {
        console.log(component.progress)
    }


    function screenchange(screenname )    {
        pageLoader.source="screens/"+screenname

    }

    Loader {
        id:pageLoader
        objectName: "loadbot"
        width: parent.width
        source:"screens/Screen_1.qml"
        anchors{
            top:hdr.bottom
            right:uiOn ? parent.right :parent.left
            bottom:ftr.top
        }

        //        Behavior on AnchorChanges{
        //            AnchorAnimation{
        //                duration: appStyle.animation_medium
        //                easing: appStyle.animation_easing
        //            }
        //        }

        focus: true
        Keys.onBackPressed: console.log("back")

        onStatusChanged: {
            if (pageLoader.status == Component.Ready){
                manager.setDceResponse("pageLoader::Command to change to:" + manager.currentScreen+ " was successfull")

            }else if (pageLoader.status == Component.Loading){
                console.log("pageLoader::loading"+manager.currentScreen+" from network"+pageLoader.progress)
            }else if(pageLoader.status===Component.Error){
                lastLoaderError="Failed to load screen "+manager.currentScreen+". Reason:\n"+pageLoader.Error
                console.log("pageLoader::Command to change to:" + manager.currentScreen + " failed!")
                screenfile = source
                manager.currentScreen="Screen_X.qml"
                pageLoader.source = "screens/Screen_x.qml"
            }
        }

        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }

        transitions: [
            Transition {
                from: "*"
                to: "*"
                AnchorAnimation{
                    duration:appStyle.animation_medium
                    easing:appStyle.animation_easing
                }
            }
        ]
    }

    function setOptions(options){
        advancedrow.setCurrentSource(options)
    }

    HomeOptions {
        id: advancedrow
        anchors{
            top:hdr.bottom
            right:parent.right
            bottom:ftr.top
        }
    }

    DefaultHeader {
        id: hdr
    }

    function setNavigation(nav){
        hdr.setNavigaton(nav)
    }
    DefaultFooter {
        id: ftr
    }

    //=================Components==================================================//
    function loadComponent(componentName)
    {
        componentLoader.source = "components/"+componentName
        if (componentLoader.status == Component.Ready)
        {
            manager.setDceResponse("Command to change to:" + componentName+ " was successfull")
        }
        else if (componentLoader.status == Component.Loading)
        {
            console.log("loading component from network")

        }
        else
        {
            console.log("Command to add: " + componentName + " failed!")
            componentFile = componentName

        }
    }

    function finishLoadingComponent (comp)
    {
        if(componentLoader.status != Component.Ready)
        {
            console.log("finishing network load")
            componentLoader.source = "components/"+comp
            console.log("Component " + comp + " loaded.")
        }
        else
        {
            finishLoadingComponent(comp)
        }

    }

    Rectangle{
        id:mask
        height: parent.height
        width: parent.width
        color: "black"
        opacity:0
        Behavior on opacity{
            PropertyAnimation{
                duration: 250
            }
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {loadComponent("NullComponent.qml")}
        }

    }

    Loader{
        id:componentLoader
        height: parent.height
        width: parent.width
        source:""
        objectName: "componentbot"
        onLoaded:  {
            console.log("checking mask")
            if(!source.toString().match("NullComponent.qml")){
                mask.opacity = .65
                console.log( source.toString()+ " is loaded")
            }else{
                console.log("hiding mask")
                mask.opacity =0
            }
        }

    }

    GenericAttributeFilter{
        id:filterTarget
        anchors.centerIn: parent
    }


    MouseArea{
        id:mst
        anchors.fill: parent
        onPressed: {
            uiOn=true
            mouse.accepted=false
            console.log("Mouse X: "+mouse.x)
            console.log("Mouse Y:"+mouse.y)
            console.log("\n")
            hideUiTimer.restart()
        }
    }

}
