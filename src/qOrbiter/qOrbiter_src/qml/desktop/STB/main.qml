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
import "../lib/components"
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

    Connections{
        target: manager
        onOrientationChanged: checkLayout()
    }

    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "fonts/Sawasdee.ttf"
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
            screen:""
        }

        ListElement{
            title:"Quick Reload"
            screen:""
        }
        ListElement{
            title:"Regenerate"
            screen:""
        }
        ListElement{
            title:"Power"
            screen:""
        }
        ListElement{
            title:"Advanced"
            screen:""
        }
        ListElement{
            title:"Computing"
            screen:""
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

    function checkLayout()
    {
        console.log("c++ slot orientation changed")
        console.log(manager.appHeight+" x " + manager.appWidth)
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

        if(hdr.activeFocus)
            pageLoader.forceActiveFocus()
        else if (pageLoader.activeFocus)
            ftr.forceActiveFocus()
        else if(ftr.activeFocus)
            hdr.forceActiveFocus()
        else
            pageLoader.forceActiveFocus()

        console.log("Header Focus::"+hdr.activeFocus)
        console.log("Loader Focus::"+pageLoader.activeFocus)
         console.log("Footer Focus::"+ftr.activeFocus)
        ftr.currentItem = -1
    }

    Item{
        id:mini_screen_saver
        anchors.fill: qmlroot

        Timer{
            id:mini_ss_timer
            interval:10000
            running: true // screensaver.active
            triggeredOnStart: true
            onTriggered:mini_screen_saver_image.source= "http://"+manager.m_ipAddress+"/lmce-admin/MediaImage.php?type=screensaver&val="+manager.getNextScreenSaverImage(mini_screen_saver_image.source)
            repeat: true
        }

        Image{
            id:mini_screen_saver_image
            height: mini_screen_saver.height
            width: mini_screen_saver.width
          //  source: "http://"+manager.m_ipAddress+"/lmce-admin/MediaImage.php?type=screensaver&val="+manager.getNextScreenSaverImage(source)
        }
        Rectangle{
            anchors.fill: parent
            color:"black"
            opacity: pageLoader .activeFocus ? .65 : .25
        }
    }
    MediaManager{
        id:dceplayer
        anchors.top: parent.top
        anchors.left:parent.left

        onFocusChanged: console.log("DCEPlayer Internal focus::"+focus)
        z:dceplayer.mediaPlaying ==false ? -5 : 0
        Component.onCompleted: {
            setWindowSize(manager.appHeight, manager.appWidth);

        }

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
        onSourceChanged:  loadin
        focus:true
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
            pageLoader.forceActiveFocus()


        }
        onActiveFocusChanged: {
            if(activeFocus)
            {  console.log("Pageloader gained active focus");  pageLoader.item.focus = true  }
            else
            { console.log("Page loader lost active focus");  pageLoader.item.focus = false}
        }
        Keys.onTabPressed:{
            console.log("Tab Focus Swap.")
            qmlroot.swapFocus()
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


