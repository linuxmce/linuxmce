import QtQuick 1.1
import "components"
import "js/ComponentLoader.js" as MyJs


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

    Component.onCompleted: {
        androidSystem.updateBuildInformation()
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

    function scaleX(x){
        return x/100*manager.appWidth
    }
    function scaleY(y){
        return y/100*manager.appHeight
    }

    //    Rectangle{
    //        anchors.fill: parent
    //        id:bgFill
    //        gradient: Gradient{
    //            GradientStop{position: 0.0; color: "Black"}
    //            GradientStop{position: .45; color: "darkgrey"}
    //            GradientStop{position: .65; color: "black"}
    //        }
    //    }

    Image {
        id: appBackground
        source: "img/bg.jpg"
        anchors.fill: parent
        property string pSource:""
        property string wSource:""

    }

    //    function updateBackground(portait, wide){
    //        appBackground.pSource = portait
    //        appBackground.wSource = wide
    //    }

    function showExitConfirm(){

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
        focus: true
        Keys.onBackPressed: console.log("back")
        onSourceChanged:  loadin
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
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
            console.log("loading component from network")
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
        MouseArea{
            anchors.fill: parent
            onClicked: {}
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
            }else  {
                console.log("hiding mask")
                mask.opacity =0
            }
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
