import QtQuick 2.0
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
    property int  mousePos: hitArea.pos
    property bool uiState:true

    signal toggleInterface()

    function checkLayout()
    {
        console.log("c++ slot orientation changed")
        console.log(manager.appWidth+" x " + manager.appHeight)
    }

    FontLoader{
        id:myFont
        name:"Sawasdee"
        source: "../default/fonts/Sawasdee.ttf"
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
    ScreenSaver{
        id:pss
        anchors.centerIn: parent
        MouseArea{
            id:hitArea
            hoverEnabled: true
            onMouseXChanged: uiState ? hideTimer.restart() : toggleUI()
            anchors.fill: parent
        }
    }

    Timer{
        id:hideTimer
        interval: 10000
        running: true
        triggeredOnStart: false
        onTriggered: {
            autoHide()
        }
    }

    function autoHide(){

        toggleUI()
    }


    function toggleUI()
    {
        mediaHeader.currentState = !mediaHeader.currentState
        if(lmceScenarios.state==="show")
        {
            lmceScenarios.state = "hide"
        }
        else
        {
            lmceScenarios.state = "show"
        }


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

    function shiftFocus(){

        if(pageLoader.focus)
        {
          //  pageLoader.focus = false
            lmceScenarios.forceActiveFocus()

        }
        else
        {
          // lmceScenarios.focus = false
            pageLoader.focus = true

        }
    }

    FocusScope{
        id:loaderScope
        width: parent.width
        height: parent.height
        focus: true
        Keys.onPressed:{

            if(event.key === Qt.Key_T){
                toggleUI()
                console.log("Key toggle")
            }
            else if(event.key ===Qt.Key_Tab)
            {
                shiftFocus();
            }
            else if(event.key===Qt.Key_Up)
            {
                pageLoader.focus = true
                lmceScenarios.focus = false
            }
            else if(event.key===Qt.Key_Down)
            {
                lmceScenarios.focus = true
                pageLoader.focus = false
            }

        }



        //floorplans

        //BottomPanel{id: advanced; anchors.top: stage.top}
        ScenarioRow{
            id:lmceScenarios;
        }


    }

    Loader {
        id:pageLoader
        objectName: "loadbot"
        focus: true
        onSourceChanged:  loadin
        onFocusChanged: {
            pageLoader.item.focus=focus
            onActiveFocusChanged: console.log("Pageloader focus::"+focus)
        }
        Keys.onTabPressed:{ shiftFocus(); console.log("Changing page loader focus")}
        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)

        }
    }
    MediaHeader{
        id:mediaHeader
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




}

