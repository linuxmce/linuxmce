import QtQuick 1.1

import "../../../skins-common/lib/components"
Item{
    id:ftr

    width: qmlroot.width
    height:scaleY(14)
    anchors.top: parent.bottom
    property bool isActive: false
    onActiveFocusChanged:{
        if(activeFocus){
            scenarioList.forceActiveFocus()
            ftr.state="showing"
            console.log("showing state")
            scenarioList.currentIndex = -1
            isActive = true
        }
        else{
            currentItem = -1
            ftr.state = "hidden"
        }
    }
    Component.onCompleted: ftr.state = "hidden"

    property int currentItem: -1
    Connections{
        target: manager
        onRoomChanged:{

        }
    }

    GradientFiller {
        opacity: .75
    }

    Item{
        id:npOptions
        width: (dcenowplaying.b_mediaPlaying ) ? scaleX(8) :0
        anchors{
            top:parent.top
            bottom:parent.bottom
            margins: 5
            left:parent.left
        }
    }

    ListView{
        id:scenarioList
        height: parent.height / 2
        anchors{
            top:parent.top
            left:npOptions.right
            right:parent.right
            margins: 5
        }
        property int itemWidth:(scenarioList.width)/6
        spacing:scaleX(1)
        orientation: ListView.Horizontal
        onActiveFocusChanged:{
            console.log("Active focus on Scenario listmodel is " + activeFocus);
            if(activeFocus){
                if(scenarioList.count === 0){

                }
            }
        }
        clip:false
        focus:false
        model:scenarios
        delegate: Item{
            id:scenarioParent
            height: parent.height
            width:scenarioList.itemWidth
            onActiveFocusChanged: {
                if(activeFocus){
                    currentItem= index
                    if(manager.currentScreen!=="Screen_47.qml"){
                        if(modelName==="currentRoomLights")
                            submodel.model = currentRoomLights
                        else if(modelName==="currentRoomMedia")
                            submodel.model = currentRoomMedia
                        else if(modelName==="currentRoomClimate")
                            submodel.model = currentRoomClimate
                        else if(modelName==="currentRoomTelecom")
                            submodel.model=currentRoomTelecom
                        else if(modelName==="currentRoomSecurity")
                            submodel.model= currentRoomSecurity
                        else if (modelName==="advancedMenu")
                            submodel.model=advancedMenu
                    }else if (manager.currentScreen==="Screen_47"){

                        if(name==="Attribute")
                            console.log("attribute selected")
                    }
                    submodel.currentIndex = -1
                }

            }

            Rectangle{
                anchors.fill: parent
                radius:5
                opacity: .75
                color: currentItem===index ? appStyle.listItemActiveBgColor : appStyle.listItemBgColor
            }

            StyledText{
                anchors.centerIn: parent
                text:name
                font.pixelSize:headerText
                font.bold: true
                font.capitalization: Font.SmallCaps
                color:appStyle.lighttext
            }

            Rectangle{
                anchors.fill: parent
                gradient: appStyle.buttonGradient
                radius:5
                opacity: .65
            }

            Keys.onTabPressed: {swapFocus();ftr.state="hidden"}
            Keys.onDownPressed: {
                if(submodel.currentIndex !==0){
                    submodel.decrementCurrentIndex()
                }
                else{
                    metarow.forceActiveFocus()
                    submodel.currentIndex = -1
                }
            }

            Keys.onUpPressed: submodel.incrementCurrentIndex()
            Keys.onEnterPressed: { pressed() }
            Keys.onPressed: {
                console.log(event.key)
                if(event.key === Qt.Key_Enter || event.key===Qt.Key_Return )
                {
                    if(submodel.currentIndex !==-1){
                        pressed()
                        currentItem = -1

                    }else{
                        manager.setCurrentScreen("Screen_"+(index+2)+".qml")
                    }


                    ftr.state = "hidden"
                }
            }
            signal pressed()


            ListView{
                id:submodel
                height:ftr.currentItem === index ?scaleY(85) : 0
                width: parent.width
                focus: true
                rotation: 180
                orientation:ListView.Vertical
                keyNavigationWraps: true
                clip:true
                anchors.bottom: parent.top
                anchors.bottomMargin: 10
                spacing:scaleY(1)
                delegate: Item{
                    rotation: 180
                    height:label.paintedHeight
                    width: scenarioList.itemWidth


                    GradientFiller{
                        fillColor: submodel.currentIndex === index ? "black" : appStyle.button_system_color
                        opacity: submodel.currentIndex === index ? .75 : .55
                        grpRadius: 5
                    }

                    StyledText {
                        id:label
                        color:appStyle.lighttext
                        text: title
                        font.bold: submodel.currentIndex === index
                        width:parent.width
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors.centerIn: parent
                        font.pointSize: submodel.currentIndex === index ? headerText : mediumText

                    }
                    Connections{
                        target:scenarioParent
                        onPressed:{

                            if(submodel.currentIndex === index && submodel.model !==advancedMenu){
                                manager.execGrp(params)
                            }else{
                                // console.log(submodel.model[index].params)
                                if(submodel.currentIndex!==index){
                                    //console.log(advancedMenu.get(index).params)
                                    return;
                                }
                                switch(params){
                                case 1:
                                    manager.quickReload()
                                    break;
                                case 2:
                                    manager.regenOrbiter(manager.iPK_Device)
                                    break;
                                case 3:
                                    console.log("Show Power!")
                                    break;
                                case 5:
                                    manager.setCurrentScreen("Screen_44.qml")
                                    break;
                                case 7:
                                    manager.exitApp()
                                    break;
                                default:
                                    console.log(params)
                                    break;
                                }
                            }
                        }
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked:{

                            if(submodel.model !==advancedMenu)
                            {
                                manager.execGrp(params)
                                currentItem = -1
                                swapFocus()
                                ftr.state = "hidden"
                            }
                            else{
                                switch(params){
                                case 1:
                                    manager.quickReload()
                                    break;
                                case 2:
                                    manager.regenOrbiter(manager.iPK_Device)
                                    break;
                                case 3:
                                    console.log("Show Power!")
                                    break;
                                case 5:
                                    manager.setCurrentScreen("Screen_44.qml")
                                    break;
                                case 7:
                                    manager.exitApp()
                                    break;
                                default:
                                    console.log(params)
                                    break;
                                }
                            }
                        }
                    }
                }

                Behavior on height {
                    PropertyAnimation{
                        duration: 500
                    }

                }
            }
            MouseArea{
                anchors.fill: parent
                onClicked: {scenarioList.currentIndex = index; forceActiveFocus()}
            }

        }


    }
    FocusRow{
        id:metarow
        height: parent.height/2
        anchors{
            top:scenarioList.bottom
            right:parent.right
            left:npOptions.right
            bottom:parent.bottom
           margins: 5
        }

        Keys.onUpPressed: scenarioList.forceActiveFocus()

        FocusButton{
            text:manager.sPK_User
            fontSize: 22
            height: parent.height
        }

        FocusButton{
            text:roomList.currentRoom+"::"+roomList.currentEA
            fontSize: 22
            height: parent.height
            width:textObj.paintedWidth
            onSelect:{ overLay.source = "RoomSelector.qml"; ftr.state = "hidden" }

        }
    }
    Clock{
        id:timekeeper
        anchors{
            top:metarow.top
            bottom: metarow.bottom
            right:metarow.right
        }

    }

    Keys.onTabPressed:swapFocus()


    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: hdr
                state:"hidden"
            }
            PropertyChanges {
                target: ftr
                currentItem:-1
                isActive:false
            }

            AnchorChanges{
                target:ftr
                anchors.bottom: undefined
                anchors.top: qmlroot.bottom

            }
        },
        State {
            name: "showing"
            PropertyChanges {
                target: hdr
                state:"showing"
                isActive:true
            }
            AnchorChanges{
                target:ftr
                anchors.top: undefined
                anchors.bottom: qmlroot.bottom
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"

            AnchorAnimation{
                duration: appStyle.globalAnimationSpeed
                easing.type: appStyle.globalAnimationEasing

            }
        }
    ]

}
