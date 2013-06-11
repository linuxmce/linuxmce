import QtQuick 2.0
import QtMultimedia 5.0
import QtSensors 5.0 as SensorArray
import QtGraphicalEffects 1.0
import "../../lib/handlers"
import "components"

Item {
    id: qml_root

    onWidthChanged: console.log(width+"::"+height)
    height:parent.height
    width:parent.width
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    property variant current_scenario_model
    property variant current_header_model:scenarios

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
    Rectangle{
        anchors.fill: nav_row
        color: "black"
        opacity: .65
    }

    Row{
        id:nav_row
        height: scaleY(8)
        width:parent.width
        spacing:scaleY(2)
        ListView{
            id:scenarioList
            height:scaleY(7)
            width:  (scaleX(10)*8)
            spacing: scaleY(2)
            model:current_header_model
            clip:true
            orientation:ListView.Horizontal
            anchors.verticalCenter: parent.verticalCenter
            delegate: Item{
                height: childrenRect.height
                width: childrenRect.width
                StyledButton{
                    id:dummy
                    buttonText.text: name
                    buttonText.color: "antiquewhite"
                    hitArea.onReleased: {
                        if(manager.currentScreen==="Screen_1.qml"){
                            if(modelName==="currentRoomLights")
                                current_scenario_model = currentRoomLights
                            else if(modelName==="currentRoomMedia")
                                current_scenario_model = currentRoomMedia
                            else if(modelName==="currentRoomClimate")
                                current_scenario_model = currentRoomClimate
                            else if(modelName==="currentRoomTelecom")
                                current_scenario_model=currentRoomTelecom
                            else if(modelName==="currentRoomSecurity")
                                current_scenario_model = currentRoomSecurity
                        }else if (manager.currentScreen==="Screen_47"){

                            if(name==="Attribute")
                                console.log("attribute selected")
                        }


                    }
                }
            }
        }
        StyledButton{
            buttonText.text:"Advanced"
            visible: manager.currentScreen === "Screen_1.qml"
        }
        StyledButton {
            id: exit_label
            buttonText.text: qsTr("Exit")
            hitArea.onReleased: manager.exitApp()
            visible:manager.currentScreen ==="Screen_1.qml"
        }
        StyledButton {
            id: home_label
            buttonText.text: qsTr("Home")
            hitArea.onReleased: manager.gotoQScreen("Screen_1.qml")
            visible: manager.currentScreen !=="Screen_1.qml"
        }
        StyledButton{
            id:media_goback
            buttonText.text: "Back"
            hitArea.onReleased:
            {
                if(manager.i_current_mediaType !== 5){
                    manager.goBackGrid();
                 }
                else{

                    pageLoader.item.state="selection"
                    manager.goBackGrid()
                   mediatypefilter.reset()
                }


            }
            visible: manager.currentScreen==="Screen_47.qml"
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
        height: qml_root.height-nav_row.height
        anchors.top: nav_row.bottom
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
