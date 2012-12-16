import QtQuick 1.1
import "components"
import "js/ComponentLoader.js" as MyJs
import "effects"

Item {
    id: item
    width:appW
    height:appH

    Rectangle{
        anchors.fill: parent

        FontLoader { id: fixedFont; name: "Krungthep" }
        FontLoader { id: localFont; source: "fonts/Krungthep.ttf" }


        gradient: Gradient {
                GradientStop { position: 0.0; color: "#dedede" }
                GradientStop { position: .85; color: "#dedede" }
            }
        Image {
            id: mainBg
            source: "background.png"
            width:appW
            height:appH
        }
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
        onSourceChanged: {opacity = 0; fadein.start()}

        PropertyAnimation{
            id: fadein
            target:pageLoader
            properties: "opacity"; to: "1"; duration: 5000
        }

    }

    SequentialAnimation{
    id:loadin

    PropertyAnimation{
        id:fadeout
        target:pageLoader
        properties: "opacity"; to: "0"; duration: 5000

    }

   }
}
