import QtQuick 2.2
import "../../../../skins-common/qt5/default/components"

//android-phone (qt5)

Item {
    id: qml_root
    height:manager.appHeight
    width: manager.appWidth

    property alias skinStyle:style
    property bool uiOn:true

    property variant current_scenario_model:currentRoomLights
    property variant current_header_model
    property string locationinfo: "standby"
    property string screenfile:""

    signal showMetadata()
    signal showPlaylist()
    signal showAdvanced()
    signal close()
    signal changeScreen(string s)
    signal setupStart(int x, string y)

    function scaleX(x){
        return x/100*qml_root.width
    }
    function scaleY(y){
        return y/100*qml_root.height
    }

    function dumpObj(obj, message){
        console.log((message? message+"\n\t" : "No Message \n\t")+ JSON.stringify(obj, null, "\t"))
    }

    function raiseNavigation(raised){
        if(raised ){
            nav_row.state="raised"
        } else {
            nav_row.state="lowered"
        }
    }


    focus:true

    Rectangle{
        anchors.fill: parent
        id:bgFill
        color:"black"
    }

    onUiOnChanged: {
        console.log("UiOn::"+uiOn)
        if(uiOn){
            if(info_panel.restore){
                info_panel.state="retracted"
                info_panel.restore=false
            }
        } else {
            if(info_panel.state==="retracted"){
                info_panel.restore=true
            } else {
                info_panel.restore=false
            }

            info_panel.state="hidden"
        }
    }



    Rectangle{
        id:canary
        height: 1
        width: 1
        color:"white"
    }
    Style{
        id:style
    }

    Keys.onReleased:{
        switch(event.key){

        case Qt.Key_Menu:
            showOptions=!showOptions
            console.log("menu button caught in root")
            break;

        case Qt.Key_Back:
            if(manager.currentScreen===1){
                showExitConfirm()
            } else{
                manager.goBacktoQScreen()
            }
            console.log("Caught Back Key")
            break
        case Qt.Key_MediaPrevious:
            if(manager.currentScreen===1){
                showExitConfirm()
            } else{
                manager.goBacktoQScreen()
            }
            break;
        case Qt.Key_VolumeDown:
            androidSystem.mediaSetVol(-1)
            console.log("vol-")
            break;
        case Qt.Key_VolumeUp:
            androidSystem.mediaSetVol(+1)
            console.log("vol+")
            break;

        default:
            console.log(event.key)
        }
        event.accepted=true
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

    property color mobilegradientone:"darkgreen"
    property color mobilegradienttwo:style.contentBgColor
    property variant colorArray:["darkblue", "black", "grey", "darkcyan" ,"purple", "slategrey", "crimson", "gold", "dodgerblue"]

    Rectangle{
        id:mobileGradient
        anchors.fill: parent
	color: "black"
        MouseArea{
            anchors.fill: parent
            onClicked: {
                if(!uiOn){
                    console.log("screensaver revive")
                    uiOn=true
                }
            }
        }
    }
    function screenchange(screenname ){
        var num = screenname.slice(7, screenname.indexOf("."))
        if(pageLoader.currentScreen!=num)
            pageLoader.nextScreen = num
    }

    function checkStatus(component){
        console.log(component.progress)
    }

    function getSkinPath(num) {
        if (style.customScreens.indexOf(num) !== -1) {
            return "./";
        } else {
            return style.commonQmlPath;
        }
    }

    function getScreen(num) {
        return getSkinPath(num) + "screens/Screen_"+(num > 0 ? num : "X")+".qml";
    }


    Loader {
        id:pageLoader
        objectName: "loadbot"
        focus: true
        source: "screens/Screen_1.qml"
        // visible:qml_root.uiOn
        property int nextScreen:1
        property int currentScreen:1
        anchors{
            top: nav_row.bottom
            bottom:info_panel.top
            left:qml_root.left
            right:qml_root.right

        }

        onNextScreenChanged: {

       /*     if(!nextScreen.match(".qml")){
                return
            } else {

            }
*/
            if(!pageLoader.item){
                console.log("Last screen likely had errors, loading next screen==>"+nextScreen)
                loadNext()
            }

            if( pageLoader.item.noForce===true){
                console.log("pageloader::"+source+" declares noforce::"+pageLoader.item.noForce+", ignoring "+nextScreen)
                return;
            }else {
                console.log("pageloader::"+source+" noforce::"+pageLoader.item.noForce+" , loading next screen.")
                console.log("next screen==>"+nextScreen)
                startChange()
            }

        }

        function startChange(){


            if(!pageLoader.item || pageLoader.item.scree){
                console.log("pageloader::closing page "+ manager.currentScreen)
                pageLoader.item.state="closing"
            } else{
                console.log("pageloader::no page jumping to next ==>"+nextScreen)
                loadNext()
            }

        }

        function loadNext(){


            if(nextScreen===""){
                nextScreen=1
                return
            }

            console.log("pageloader::loading next screen::\n"+getScreen(nextScreen)) //style.commonQmlPath+"screens/"+nextScreen)
            pageLoader.source=getScreen(nextScreen) //style.commonQmlPath+"screens/"+nextScreen
        }

        opacity: uiOn ? 1 : 0
        Behavior on opacity {
            PropertyAnimation{
                duration: 750
            }
        }

        Keys.onBackPressed: console.log("back")

        onLoaded: {
            console.log("Screen Changed:" + pageLoader.source)
        }
        onStatusChanged:  if (pageLoader.status == Component.Ready)
                          {

                              manager.setDceResponse("Command to change to:" + source+ " was successfull")
                              screenfile = source
                              currentScreen=nextScreen

                              // contentItem=item.screen_root
                          }
                          else if (pageLoader.status == Component.Loading)
                          {
                              console.log("loading page from network")

                          }
                          else if(pageLoader.status == Component.Error)
                          {
                              console.log("Command to change to:" + source + " failed!")
                              manager.currentScreen=0
                          }
    }
    NavigationRow {
        id: nav_row
        rowheight: style.rowH
        navSource: "../../../../android/phone/qt5/default/components/PhoneNavHome.qml"
        commonButtonVisible: pageLoader.currentScreen != 1
    }

    function showRoomSelector() {
	info_panel.state = "room"
    }

    InformationPanel {
        id: info_panel
        defaultState: "minimal"
    }

    function hideInfoPanel(){
        info_panel.state = "hidden"
    }

    function showInfoPanel(){
        info_panel.state = "retracted"
    }

    function setNavigation(n){
        nav_row.navSource=""
        nav_row.navSource = n
    }

    function updateBackground(portait, wide){
        appBackground.pSource = portait
        appBackground.wSource = wide
    }

    FontLoader{
        id:appFont
        source:".././../../../skins-common/fonts/Sawasdee.ttf"
    }

    //=================Components==================================================//
    function loadComponent(componentName)
    {
        componentLoader.source = "components/"+componentName
        if (componentLoader.status == Component.Ready)
        {
            manager.setDceResponse("Command to load:" + componentName+ " was successfull")

        }
        else if (componentLoader.status == Component.Loading)
        {
            console.log("loading item from network")
            finishLoadingComponent(componentName)
        }
        else
        {
            console.log("Command to load: " + componentName + " failed!")
            componentFile = componentName
        }
    }

    function finishLoadingComponent (comp)
    {
        if(componentLoader.status != Component.Ready)
        {
            console.log("finishing network load")
            componentLoader.source = style.commonQmlPath+"components/"+comp
            console.log("item " + comp + " loaded.")
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
        onLoaded: {
            console.log("Component is loaded")
        }
    }



    MouseArea{
        id:mst
        anchors.fill: parent

        onPressed: {
            mouse.accepted=false
            console.log("Mouse X: "+mouse.x)
            console.log("Mouse Y:"+mouse.y)
            console.log("\n")
        }
    }
}
