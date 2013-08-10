import QtQuick 1.1
import "components"
import "js/ComponentLoader.js" as MyJs

Item {
    id: item
    width:manager.appWidth
    height:manager.appHeight
    focus:true
    Connections{
        target:manager
        onBackButtonPressed:console.log("Go Back Button!")
    }
    Keys.onPressed: if(event.key ===Qt.Key_MediaPrevious) {
                        executeBack();
                    } else if(event.key === Qt.Key_Menu){
                        console.log("menu button!")
                    } else if(event.key===Qt.Key_VolumeUp){
                        console.log("Volume up button!")
                    } else if (event.key===Qt.Key_VolumeDown){
                        console.log("Volume Down button!")
                    }
                    else {console.log(event.key)}

    function executeBack(){
        componentLoader.source="components/ExitConfirm.qml"
    }

    //    Rectangle{
    //        anchors.fill: parent
    //        gradient: Gradient {
    //            GradientStop { position: 0.0; color: "darkslategrey" }
    //            GradientStop { position: .85; color: "slategrey" }
    //        }
    //    }

    //    ScreenSaver{
    //        id:ss
    //        anchors.fill: parent
    //        MouseArea{
    //            anchors.fill: parent
    //            onClicked: pageLoader.opacity === 0 ?  pageLoader.opacity = 1 : uiTimer.restart()
    //        }
    //    }


    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../fonts/Sawasdee.ttf"
    }

    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    Image{
        source:"img/bg.jpg"
        anchors.fill: parent
    }

    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)


    property string locationinfo: "standby"
    property string screenfile
    property string dynamic_height
    property string dynamic_width

    function checkLayout()
    {
        console.log("c++ slot orientation changed")
    }

    Connections{
        target: manager
        onOrientationChanged: checkLayout()
    }

    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }


    function screenchange(screenname )
    {

        componentLoader.source = ""
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


    Loader {
        id:pageLoader
        objectName: "loadbot"
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
        Behavior on opacity{
            PropertyAnimation{
                duration: 1000
            }
        }
        Keys.onPressed: {
            console.log(event.key)
        }
    }

    Loader{
        id:popupLoader
        anchors.centerIn: parent
        z:2
    }

    //=================Components==================================================//
    function loadComponent(componentName )
    {
        componentLoader.source = "components/"+componentName
        if (componentLoader.status == Component.Ready)
        {
            manager.setDceResponse("Command to change to:" + componentName+ " was successfull")
        }
        else if (componentLoader.status == Component.Loading)
        {
            console.log("loading Component from network")
            finishLoadingComponent(componentName)
        }
        else
        {
            console.log("Command to add: " + componentName + " failed!")
            componentFile = componentName

        }
    }

    function finishLoadingComponent (componentName)
    {
        if(componentLoader.status != Component.Ready)
        {
            console.log("Finished loading network component")
            componentLoader.source = "components/"+componentName
            console.log("screen" + componentName + " loaded.")
        }
        else
        {
            finishLoadingComponent(componentName)
        }

    }


    Loader{
        id:componentLoader
        height: parent.height
        width: parent.width
        objectName: "componentbot"
        onLoaded: {console.log("Component is loaded")}
        onSourceChanged: {opacity = 1}
    }

    PropertyAnimation{
        id:fadeout
        target:pageLoader
        properties: "opacity"; to: "0"; duration: 500
    }
    PropertyAnimation{
        id:fadein
        target:pageLoader
        properties: "opacity"; to: "1"; duration: 500
    }

}
