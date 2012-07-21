import QtQuick 1.1
import Qt.labs.shaders 1.0
import "effects"
import "components"
import "js/ComponentLoader.js" as MyJs


Item {
    id: item
  width:appW
    height:appH

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

DebugPanel{
    id:dcemessages
    debugMessage: dcemessage
    z:2
}

DebugPanel{
    id:mediaMessages
    debugMessage: dcerouter.mediaResponse
    z:2
}

DebugPanel{
    id:commandmessages
    debugMessage: dcerouter.commandResponse
    z:2
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
    ScreenSaver
    {   id:ss
        height: appH
        width: appW
        anchors.centerIn: parent
    }
    function scaleX(x){
    return x/100*appW
    }
    function scaleY(y){
    return y/100*appH
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


    Loader {
    id:pageLoader
    objectName: "loadbot"

    onSourceChanged:  loadin
    onLoaded: {

        console.log("Screen Changed:" + pageLoader.source)

        }
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
        if(componentLoader.status != Component.Ready)
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

    //-----------------------------shader related---------------------------//


    //floorplans




}
