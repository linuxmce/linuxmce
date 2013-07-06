import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../../lib/components"
Item{
    id:ftr

    width: qmlroot.width
    height:scaleY(12)
    anchors.top: parent.bottom
    onActiveFocusChanged:{ if(activeFocus){
            scenarioList.forceActiveFocus()
            ftr.state="showing"
            console.log("showing state")
            scenarioList.currentIndex = -1
        }
        else{
            currentItem = -1
            ftr.state = "hidden"
            pageLoader.item.focus = true
        }
    }
    Component.onCompleted: ftr.state = "hidden"

    property int currentItem: -1


    Rectangle{
        anchors.fill: parent
        opacity:ftr.activeFocus ? 1 : .65
        color:appStyle.mainColor
        border.width: ftr.activeFocus ? 2 : 0
        border.color:  "red"
    }

    ListView{
        id:scenarioList
        height: parent.height / 2
        width: parent.width
        spacing:scaleX(1)
        orientation: ListView.Horizontal
        onActiveFocusChanged:{ console.log("Active focus on Scenario listmodel is " + activeFocus); forceActiveFocus()}
        clip:false
        focus:false
        model:scenarios
        delegate: Item{
            id:scenarioParent
            height: parent.height
            width: scaleX(18)
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
                color: currentItem===index ? appStyle.lightHighlightColor : appStyle.darkHighlightColor
            }
            StyledText{
                anchors.centerIn: parent
                text:name
                font.pixelSize: 32
            }
            Keys.onTabPressed: {swapFocus();ftr.state="hidden"}
            Keys.onDownPressed:  submodel.decrementCurrentIndex()
            Keys.onUpPressed: submodel.incrementCurrentIndex()
            Keys.onEnterPressed: { pressed() }
            Keys.onPressed: {
                console.log(event.key)
                if(event.key === 16777220 )
                {
                    pressed()
                    currentItem = -1
                   swapFocus()
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
                spacing:scaleY(1)
                delegate: Item{
                    rotation: 180
                    height:label.paintedHeight
                    width: parent.width
                    Rectangle{
                        anchors.fill: parent
                        radius:5
                        clip: true
                        color:submodel.currentIndex === index ? "darkgrey" : "black"
                        opacity: submodel.currentIndex === index ? .85 : .65
                    }

                    StyledText {
                        id:label
                        color:"white"
                        text: title
                        font.bold: submodel.currentIndex === index
                        width:parent.width
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors.centerIn: parent
                        font.pointSize: submodel.currentIndex === index ? headerText : mediumText

                    }
                    Connections{
                        target:scenarioParent
                        onPressed:if(submodel.currentIndex === index) manager.execGrp(params)
                    }

                }

                Behavior on height {
                    PropertyAnimation{
                        duration: 500
                    }

                }
            }
        }

    }
    Row{
        id:metarow
        height: parent.height/3
        width: parent.width
        anchors.bottom: parent.bottom
        Item{
            id:userInfo
            width: parent.width/3
            height: parent.height
            StyledText {
                anchors.centerIn: parent
                text: manager.sPK_User
                fontSize: headerText
                color:appStyle.accentcolor
            }

        }
        Item{
            id:locationInfo
            width: parent.width/3
            height: parent.height

            StyledText {
                id: loc
                text: roomList.currentRoom+"::"+roomList.currentEA
                fontSize: mediumText
                color:appStyle.accentcolor
            }
        }
        Clock{
            id:timekeeper
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
