import QtQuick 1.1
import "components"
import "js/ComponentLoader.js" as MyJs

Item {
    id: item
    width:manager.appWidth
    height:manager.appHeight

    //    Rectangle{
    //        anchors.fill: parent
    //        gradient: Gradient {
    //            GradientStop { position: 0.0; color: "darkslategrey" }
    //            GradientStop { position: .85; color: "slategrey" }
    //        }
    //    }

    ScreenSaver{
        id:ss
        anchors.fill: parent
        MouseArea{
            anchors.fill: parent
            onClicked: pageLoader.opacity === 0 ?  pageLoader.opacity = 1 : uiTimer.restart()
        }
    }
    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../../fonts/Sawasdee.ttf"
    }

    Timer{
        id:uiTimer
        interval: 60000
        onTriggered: pageLoader.opacity = 0
        running: pageLoader.opacity > 0
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
    /*
    Image {
    id: bg
    source: "img/icons/backgrounds/livingroom.png"
    anchors.fill:parent
    }
*/

    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }


    function screenchange(screenname )
    {
        fadeout.start()
        componentLoader.source = ""
        pageLoader.source = "screens/"+screenname

        if (pageLoader.status == Component.Ready)
        {fadein.start()
            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
        }
        else if (pageLoader.status == Component.Loading)
        {
            console.log("loading page from network")
            finishLoading(screenname)
        }
        else
        {fadein.start()
            console.log("Command to change to:" + screenname + " failed!")
            screenfile = screenname
            pageLoader.source = "screens/Screen_x.qml"
            uiTimer.restart()
        }
    }

    function finishLoading (screenname)
    {
        if(pageLoader.status != Component.Ready)
        {
            console.log("finishing load")
            pageLoader.source = "screens/"+screenname
            fadein.start()
            console.log("screen" + screenname + " loaded.")
            uiTimer.restart()
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
        Keys.onPressed: { uiTimer.restart() ;
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
