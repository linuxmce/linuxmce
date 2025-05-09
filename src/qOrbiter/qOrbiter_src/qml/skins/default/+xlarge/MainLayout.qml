import QtQuick 2.2
//import QtGraphicalEffects 1.0
//import AudioVisual 1.0
import org.linuxmce.screens 1.0
import "."
import "../."
import "../components"


/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot

    property bool uiOn:false
    property alias scenarioModel:current_scenarios.model
    focus:true
    activeFocusOnTab: false


    //    onUiOnChanged: {
    //        if(uiOn)
    //           // qmlRoot.resetTimeout()
    //        else
    //            qmlRoot.screenSaverActivated()
    //    }

    function raiseNavigation(raise){
        uiOn=raise;
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
        manager.m_bIsOSD = true

    }
    onActiveFocusChanged: {
        console.log("Layout has focus ? "+ activeFocus)
        if(pageLoader.item)
            pageLoader.item.forceActiveFocus()
    }

    Keys.onEscapePressed: uiOn=false

    Keys.onTabPressed: {

        console.log("Shift Focus")
        if(layout.activeFocus){
            qmlRoot.resetTimeout();
            header.forceActiveFocus()
        } else if (header.activeFocus){
            footer.forceActiveFocus()
        } else {
            layout.forceActiveFocus()

        }
    }

    Connections{
        target: dcenowplaying
        onMediaStatusChanged:{
            console.log("Media Status changed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Status is "+dcenowplaying.b_mediaPlaying)
            if(dcenowplaying.b_mediaPlaying){
                qmlPictureFrame.stopScreenSaver();
            } else {
                qmlPictureFrame.startScreenSaver()
                qmlPictureFrame.getNextImage()
            }


        }
    }

    QmlPictureFrame {
        id: qmlPictureFrame

        MouseArea{
            anchors.fill: parent
            onClicked: uiOn=!uiOn
        }

    }

    MediaInterface{
        id:mediaPlayer
        anchors.centerIn: parent
        height:parent.height
        width:parent.width

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
        property int currentItem:-1

        onActiveFocusChanged:{
            if(activeFocus){
                uiOn=true ; currentItem=0; active=true
            } else {
                currentItem=0
            }

        }
        onCurrentItemChanged: {
            topControls.children[currentItem].forceActiveFocus()
        }

        Keys.onLeftPressed: { if(currentItem==0)return; currentItem--}
        Keys.onRightPressed: {if(currentItem===2)return; currentItem++}
        Keys.onDownPressed:{ footer.forceActiveFocus() }
        Keys.onEscapePressed: layout.forceActiveFocus()

        Row{
            id:topControls
            focus:true
            anchors{
                right:header.right
                top:parent.top
                bottom:parent.bottom
                margins:10
            }

            spacing: appStyle.buttonSpacing

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

        height: current_scenarios.count * appStyle.scaleY(13)
        width:appStyle.scaleX(15)
        anchors.bottom: footer.top
        anchors.bottomMargin: 5
        visible: footer.activated

        ListView{
            id:current_scenarios
            anchors.fill: parent
            spacing:5
            anchors.margins: appStyle.buttonSpacing
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
                height:appStyle.scaleY(13)
                width:appStyle.scaleX(15)
                onActivated: {
                    current_scenarios.currentIndex= index;
                    execute();
                }

                //                MouseArea{
                //                    anchors.fill: parent
                //                    onClicked: { scenario_delegate.acivated()}
                //                }
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
                if(activeFocus)scenarioList.forceActiveFocus()
            }

        }

        LargeStyledButton{
            id:npButton
            buttonText:dcenowplaying.qs_mainTitle
            arrow:false
            visible: dcenowplaying.b_mediaPlaying
           anchors.left: parent.left
           anchors.leftMargin: visible ? 0 : width*-1

            onActivated:{
                console.log("blip")
              manager.currentScreen = dcenowplaying.qs_screen

            }
        }

        ListView{
            id:scenarioList
            Keys.priority:Keys.BeforeItem
            focus:true
            spacing: 5
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
                left:npButton.right
                right:parent.right
                bottom:parent.bottom
                margins: 5
            }
            Keys.onPressed: {
                switch(event.key){
                case Qt.Key_Enter: console.log("enter ");
                case Qt.Key_Return:console.log("return key"); current_scenarios.currentItem.activated(); event.accepted=true; console.log("command executed") ;break;
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
                buttonText:name
                arrow:false


                Keys.forwardTo: [ scenarioList ]

                onActiveFocusChanged:{
                    if(activeFocus){
                        scenarioList.setModel(floorplantype)
                        centralScenarios.x = x
                    }
                }
                onActivated:{
                    forceActiveFocus()
                    if(floorplantype===-1)
                        manager.currentScreen="Screen_44.qml";scenarioList.currentIndex=0;
                }
            }
        }
    }
}
