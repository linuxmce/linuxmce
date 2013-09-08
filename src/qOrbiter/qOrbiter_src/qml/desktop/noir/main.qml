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
import "../../skins-common/lib/components"
import "../../skins-common/lib/handlers"
import "components"
import "js/ComponentLoader.js" as MyJs

Item {
    id: item
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

    function checkUi(){
        console.log("Checking focused items")
        if(lmceScenarios.focus === true){
            pageLoader.focus = true
        }
        else
        {
            lmceScenarios.focus = true
        }
    }

    function toggleUI()
    {
        if(lmceScenarios.focus)
            stageScope.focus = true
        else
            lmceScenarios.focus = true

    }



    function checkLayout()
    {
        console.log("c++ slot orientation changed")
        console.log(manager.appHeight+" x " + manager.appWidth)
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

    Style{
        id:mystyle
    }

    FontLoader{
        id:fontstyle
        source: "fonts/Sawasdee.ttf"
    }

//    ScreenSaver{
//        id:pss
//        anchors.centerIn: parent
//    }

    FocusScope
    {
        id:stageScope
        height: manager.appHeight
        width: manager.appWidth
        focus:true

        MediaHeader{
            id:mediaHeader
            anchors.top: parent.top
            focus:false
            opacity: lmceScenarios.opacity
        }


        //BottomPanel{id: advanced; anchors.top: stage.top}

        Loader {
            id:pageLoader
            objectName: "loadbot"
            onSourceChanged:  loadin

            onLoaded: {
                console.log("Screen Changed:" + pageLoader.source)
                pageLoader.item.focus = true
            }

            onFocusChanged: {console.log("Focus for page loader is now "+focus);pageLoader.item.focus = focus}
            onActiveFocusChanged: console.log("ActiveFocus for page loader is now " +activeFocus)
            Keys.onTabPressed: {lmceScenarios.showingMenu=true; pageLoader.item.focus = false ; console.log("Pageloader tab press")}

            PropertyAnimation{
                id:wobble
                target: pageLoader
                property: "rotation"
                from:15
                to:270
                duration: 500
                loops: 5
                onCompleted: pageLoader.rotation = 0
            }
        }

        ScenarioRow{
            id:lmceScenarios;
            anchors.bottom: parent.bottom
            Keys.onTabPressed: {pageLoader.focus = true; showingMenu =false}
        }
        Loader{
            id:componentLoader
            height: parent.height
            width: parent.width
            objectName: "componentbot"
            onLoaded: {console.log("Component is loaded"); focus = true}
            onFocusChanged: console.log("Focus for Component loader is now "+focus)
            onActiveFocusChanged: console.log("ActiveFocus for Component loader is now " +activeFocus)
            focus:false
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

    //-----------------------------shader related---------------------------//


    //floorplans



}

