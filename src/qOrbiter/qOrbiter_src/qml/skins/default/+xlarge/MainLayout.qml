import QtQuick 2.2
import QtGraphicalEffects 1.0
import AudioVisual 1.0
import org.linuxmce.screens 1.0
import "."
import "../."
import "components"


/*! This File is designed to be the main layout that can be switched in and out for various forms */
Item {
    id:layout
    anchors.fill: qmlRoot

    property bool uiOn:true
    property alias scenarioModel:current_scenarios.model

    function raiseNavigation(raise){
        uiOn=raise;
    }

    Component{
        id:compList
        RoomSelector {
            id:room_selector
        }
    }

    Component.onCompleted: forceActiveFocus()
    focus:true
    activeFocusOnTab: false
    onActiveFocusChanged: {
        console.log("Layout has focus ? "+ activeFocus)
        if(pageLoader.item)
            pageLoader.item.forceActiveFocus()
    }

    Keys.onEscapePressed: uiOn=false

    Keys.onTabPressed: {
        uiOn=true
        console.log("Shift Focus")
        if(layout.activeFocus){
            header.forceActiveFocus()
        } else if (header.activeFocus){
            footer.forceActiveFocus()
        } else {
            layout.forceActiveFocus()
        }
    }



    QmlPictureFrame {
        id: qmlPictureFrame
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
        property int currentItem:-1

        onActiveFocusChanged:{
            if(activeFocus){
                uiOn=true
                currentItem=0
                active=true
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

            delegate: LargeStyledButton{
                id:scenario_delegate
                function execute(){
                    console.log(title+" is executing")
                    manager.execGrp(params)
                }
                arrow:current_scenarios.currentIndex===index

                currentSelection:arrow
                buttonText:title
                height:Style.scaleY(13)
                width:Style.scaleX(15)
//                Keys.onEnterPressed: manager.execGrp(params)
//                Keys.onReturnPressed: manager.execGrp(params)
                MouseArea{
                    anchors.fill: parent
                    onClicked: execute()
                }
            }
        }
    }

    Footer {
        id: footer

        focus:true
        onActiveFocusChanged:{
            header.active=false
            if(activeFocus)scenarioList.forceActiveFocus()
        }

        ListView{
            id:scenarioList

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
                left:parent.left
                right:parent.right
                bottom:parent.bottom
                margins: 5
            }
            Keys.onUpPressed: {
                current_scenarios.decrementCurrentIndex()
            }

            Keys.onDownPressed: {
                current_scenarios.incrementCurrentIndex()
            }
            Keys.onDigit1Pressed: {
                current_scenarios.currentItem.execute()
            }

            orientation: ListView.Horizontal
            model:qmlRoot.scenarios
            delegate:  LargeStyledButton{
                id:btn
                buttonText:name
                arrow:true
                onActiveFocusChanged:{

                    if(activeFocus){
                        scenarioList.setModel(floorplantype)
                        console.log(btn.x)
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
