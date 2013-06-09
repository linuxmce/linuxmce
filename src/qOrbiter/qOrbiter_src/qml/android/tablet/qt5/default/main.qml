import QtQuick 2.0
import QtMultimedia 5.0
import QtSensors 5.0 as SensorArray
import QtGraphicalEffects 1.0


Item {
    id: qml_root


    onWidthChanged: console.log(width+"::"+height)
    height:parent.height
    width:parent.width
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width

    function scaleX(x){
        return x/100*qml_root.width
    }
    function scaleY(y){
        return y/100*qml_root.height
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

    Image {
        id: appBackground
        source: manager.b_orientation ? pSource : wSource
        anchors.fill: parent
        property string pSource:""
        property string wSource:""
    }

    Item{
        id:mini_screen_saver
        anchors.fill: qml_root

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
            source: "http://"+manager.m_ipAddress+"/lmce-admin/MediaImage.php?type=screensaver&val="+manager.getNextScreenSaverImage(source)
        }
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
        name:"Sawasdee"
        source:"../../../fonts/Sawasdee.ttf"
    }


    Loader {
        id:pageLoader
        objectName: "loadbot"
        focus: true
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
            console.log("loading page from network")
            finishLoadingComponent(componentName)
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
            console.log("screen" + comp + " loaded.")
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
        onLoaded: {console.log("Component is loaded")}
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
