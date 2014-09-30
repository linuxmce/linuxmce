import QtQuick 2.0
import DceScreenSaver 1.0
import "components"
import "js/ComponentLoader.js" as MyJs


Item {
    id: qml_root
    width:manager.appWidth
    height:manager.appHeight
    focus:true

    Style{
        id:appStyle
    }

    FontLoader{
        id:appFont
        name: "Sawasdee"
        source:"../../../../skins-common/fonts/Sawasdee.ttf"
    }

    DceScreenSaver{
        id:glScreenSaver
        height:qml_root.height
        width: qml_root.width
        interval:30000
        anchors.centerIn: qmlroot
        active:manager.m_ipAddress = manager.internalHost
        requestUrl:manager.m_ipAddress

        Component.onCompleted: {
            if(glScreenSaver.pictureCount===0 && glScreenSaver.active){
                glScreenSaver.setImageList(manager.screensaverImages)
            }
        }
    }


//    Component.onCompleted: {
//        androidSystem.updateBuildInformation()
//    }

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

    Connections{
        target:manager
        onChangeScreen:screenchange(screenname)
    }

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



    //    function updateBackground(portait, wide){
    //        appBackground.pSource = portait
    //        appBackground.wSource = wide
    //    }

    function showExitConfirm(){

    } 


    function screenchange(screenname)
    {
        pageLoader.source = "screens/"+screenname
//        if (pageLoader.status == Component.Ready)
//        {
//            manager.setDceResponse("Command to change to:" + screenname+ " was successfull")
//        }
//        else if (pageLoader.status == Component.Loading)
//        {
//            console.log("loading page from network")
//            finishLoading(screenname)
//        }
//        else
//        {
//            console.log("Command to change to:" + screenname + " failed!")

//            screenfile = screenname
//            pageLoader.source = "screens/Screen_x.qml"
//        }
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
        source:"screens/Screen_1.qml"
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

//    Rectangle{
//        id:mask
//        height: parent.height
//        width: parent.width
//        color: "black"
//        opacity:0
//        MouseArea{
//            anchors.fill: parent
//            onClicked: {}
//        }

//    }

    Loader{
        id:componentLoader       
        source:""
        objectName: "componentbot"
        onSourceChanged:  {
            if(source!==""){
             //   mask.opacity = .65
                console.log("Component is loaded")
            }else {
             //   mask.opacity=0
            }
        }
    }


}
