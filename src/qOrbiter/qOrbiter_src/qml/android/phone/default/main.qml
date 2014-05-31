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

    Style{
        id:skinStyle
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
            console.log("initializing qml media player::"+manager.mediaPlayerID)

            if(manager.mediaPlayerID !== -1){
                dceplayer.setConnectionDetails(manager.mediaPlayerID, manager.m_ipAddress)
                androidSystem.startAudioService(dceplayer.callbackAddress);

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

    Component.onCompleted: {
        androidSystem.updateBuildInformation()
        manager.setDceGridSep(100)
    }

    Keys.onReleased: {
        switch(event.key){

        case Qt.Key_Back:
            if(manager.currentScreen==="Screen_1.qml"){
                showExitConfirm()
            } else{
                manager.goBacktoQScreen()
            }
            break
        case Qt.Key_MediaPrevious:
            if(manager.currentScreen==="Screen_1.qml"){
                showExitConfirm()
            } else{
                manager.goBacktoQScreen()
            }
            break
        default:
            console.log(event.key)
        }
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

    DceScreenSaver{
        id:glScreenSaver
        height:qmlroot.height
        width: qmlroot.width
        interval:30000
        anchors.centerIn: qmlroot
        // active: manager.m_ipAddress==="192.168.80.1"
        requestUrl:manager.m_ipAddress
        Component.onCompleted: {
            if(manager.m_ipAddress==="192.168.80.1"){
                glScreenSaver.setImageList(manager.screensaverImages)
            } else{
                active=false;
            }
        }

        Connections{
            target:manager
            onScreenSaverImagesReady:{
                glScreenSaver.setImageList(manager.screensaverImages)
                console.log("Orbiter Consume Screensaver images")
                console.log("Orbiter counts " + glScreenSaver.pictureCount)
            }
        }
    }

    Rectangle{
        id:mobileGradient
        anchors.fill: parent
        //  visible:!glScreenSaver.active
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
            color:skinStyle.toolbarBgColor

        }
    }




    function showExitConfirm(){

    }


    function screenchange(screenname )
    {
        if(manager.currentScreen===screenname){
            console.log("Duplicate call to same screen ==>"+screenname)
            //   return
        }

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
            console.log("SCREEN==>" + screenname + " loaded.")
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



    Loader {
        id:pageLoader
        objectName: "loadbot"
        anchors{
            top:hdr.bottom
            left:parent.left
            right:parent.right
            bottom:ftr.top
        }

        focus: true
        Keys.onBackPressed: console.log("back")
        onSourceChanged:  loadin
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
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
