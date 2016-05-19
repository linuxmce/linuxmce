import QtQuick 2.2
import QtGraphicalEffects 1.0
import org.linuxmce.screens 1.0
import "."
import "../."
import "../components"


/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot
    objectName: "mainlayoutobject"



    property bool uiOn:false
    property alias scenarioModel:current_scenarios.model
    focus:true
    activeFocusOnTab: false

        onUiOnChanged: {
            if(uiOn)
                qmlRoot.resetTimeout()
            else
                qmlRoot.screenSaverActivated()
        }

    function raiseNavigation(raise){
        uiOn=raise;

//        if(!raise)
//            layout.forceActiveFocus()
//        else
//            header.active=true
    }

    function menuToggle(){

        console.log("Shift Focus")
        if(layout.activeFocus){
            qmlRoot.resetTimeout();
            header.forceActiveFocus()
        } else if (header.active){
            footer.forceActiveFocus()
        } else {
            layout.forceActiveFocus()
        }
    }

    Connections{
        target: qmlRoot
        onScreenSaverActivated:{
            console.log("screen saver activated")
            uiOn=false
            pageLoader.toggleContent(false)
        }
        onResetTimeout:{
            pageLoader.toggleContent(true)
            uiOn=true
        }
        onUnhandledKey: {
            console.log("Recovering key...")
            if(key==Qt.Key_Enter){
                uiOn=true
            } else {
                console.log("Not handling recovered Key")
            }
        }

    }

    Component{
        id:compList
        RoomSelector {
            id:room_selector
        }
    }

    Component.onCompleted:{
        forceActiveFocus()
    }

    onActiveFocusChanged: {
        console.log("Layout has focus ? "+ activeFocus)
        if(activeFocus){
            if(pageLoader.item)
                pageLoader.item.forceActiveFocus()
        }
    }

    Keys.onTabPressed: { menuToggle() }
    Keys.onMenuPressed: {menuToggle()}

    QmlPictureFrame {
        id: qmlPictureFrame
        visible:!dcenowplaying.b_mediaPlaying
        MouseArea{
            anchors.fill: parent
            onClicked: uiOn=!uiOn
        }

    }

    PageLoader {
        id: pageLoader
        focus:true
        anchors{
            top:header.bottom
            left:layout.left
            right:layout.right
            bottom:layout.bottom
        }
    }


    DefaultHeader {
        id: header
        focus:true
        objectName: "+md header"
        property int currentItem:-1

        function moveLeft(){
            if(header.currentItem==0)return; header.currentItem--
        }
        function moveRight(){
            if(header.currentItem===2)return; header.currentItem++
        }

        function jumpToFooter(){
            footer.forceActiveFocus()
        }

        function hideMenu(){
            layout.forceActiveFocus()
        }

        onActiveChanged: {
            if(active)
                currentItem=0
        }

        onActiveFocusChanged:{
            if(activeFocus){
                uiOn=true ;
                active=true
                currentItem=0;

            } else {
                currentItem=0
            }

        }
        onCurrentItemChanged: {
            topControls.children[currentItem].forceActiveFocus()
        }

        Keys.onLeftPressed: { header.moveLeft() }
        Keys.onRightPressed: {header.moveRight() }
        Keys.onDownPressed:{ header.jumpToFooter() }
        Keys.onBackPressed: header.hideMenu()

        Row{
            id:topControls
            focus:true
            anchors{
                right:header.right
                top:parent.top
                bottom:parent.bottom
                margins:10
            }

            spacing: Style.buttonSpacing

            LargeStyledButton{
                buttonText: qsTr("Home")
                onActivated: manager.setCurrentScreen("Screen_1.qml")
            }

            LargeStyledButton{
                id:room_display
                buttonText: roomList.currentRoom
                onActivated:{
                    qmlRoot.createPopup(compList)
                }
            }

            LargeStyledButton{
                buttonText:manager.sPK_User
            }
        }
    }

    Item{
        id:centralScenarios
        height: current_scenarios.count * Style.scaleY(13)
        width:Style.scaleX(15)
        anchors.bottom: footer.top
        anchors.bottomMargin: 5
        visible: footer.activated

        ListView{
            id:current_scenarios
            anchors.fill: parent
            spacing:5
            anchors.margins: Style.buttonSpacing
            property int commandToExecute:-1

            function executeItem(itemIndex){
                // children[commandToExecute].execute()
                manager.execGrp(commandToExecute)
                commandToExecute=-1
                currentIndex=-1
            }

            delegate: LargeStyledButton{
                id:scenario_delegate
                focus:true

                onCurrentSelectionChanged:{
                    current_scenarios.commandToExecute=params
                }

                function execute(){
                    console.log(title+" is executing")
                    manager.execGrp(params)
                }
                arrow:current_scenarios.currentIndex===index
                currentSelection:arrow
                buttonText:title
                height:Style.scaleY(13)
                width:Style.scaleX(15)
                MouseArea{
                    anchors.fill: parent
                    onClicked: execute()
                }
            }
        }
    }

    Footer {
        id: footer
        state: header.state
        focus:true

        onActiveFocusChanged:{
            if(activeFocus){
                header.active=false
                scenarioList.currentIndex=0;
                scenarioList.forceActiveFocus()
            }
        }

        ListView{
            id:scenarioList
            Keys.priority:Keys.BeforeItem
            focus:true
            spacing: 5
            onCurrentIndexChanged: {
                console.log(currentIndex+ " << current index")
            }

            onActiveFocusChanged: {
                if(activeFocus)
                    footer.activated=true
                else
                    footer.activated = false
            }

            function setModel(modelType){
                scenarioModel=undefined
                switch(modelType) {
                case 2:scenarioModel=currentRoomLights; break;
                case 5:scenarioModel=currentRoomMedia; break;
                case 1:scenarioModel=currentRoomClimate; break
                case 3:scenarioModel=currentRoomTelecom; break
                case 4:scenarioModel=currentRoomSecurity; break;
                default:scenarioModel= undefined;
                }
            }

            anchors{
                top:parent.top
                left:parent.left
                right:parent.right
                bottom:parent.bottom
                margins: 5
            }
            Keys.onPressed: {
                switch(event.key){
                case Qt.Key_Enter: console.log("enter ");
                case Qt.Key_Return:console.log("return key"); current_scenarios.currentItem.execute(); event.accepted=true; console.log("command executed") ;break;
                case Qt.Key_1: current_scenarios.currentItem.execute(); event.accepted=true; console.log("command executed") ;break;
                case Qt.Key_Down: console.log("down key"); event.accepted=true;current_scenarios.incrementCurrentIndex();break;
                case Qt.Key_Up: console.log("up arrow"); event.accepted=true;  current_scenarios.decrementCurrentIndex(); break;
                default: console.log(event.key); break;
                }
            }



            Component.onCompleted: {
                scenarios.append({
                                     "name":"Advanced",
                                     "modelName":5,
                                     "floorplantype":-1}
                                 )
            }


            orientation: ListView.Horizontal
            model:qmlRoot.scenarios
            delegate:  LargeStyledButton{
                id:btn
                objectName: name
                buttonText:name
                arrow:false


                Keys.forwardTo: [ scenarioList ]

                onActiveFocusChanged:{
                    if(activeFocus){
                        console.log(name +" has focus")
                        scenarioList.setModel(floorplantype)
                        centralScenarios.x = x
                    }
                }
                onActivated:{
                    console.log(name + " was activated")
                    forceActiveFocus()
                    if(floorplantype===-1){
                        manager.currentScreen="Screen_44.qml";
                        scenarioList.currentIndex=0;
                    }
                }
            }
        }
    }
}
