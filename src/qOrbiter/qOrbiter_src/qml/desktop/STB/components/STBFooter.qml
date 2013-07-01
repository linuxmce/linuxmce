import QtQuick 1.1
import Qt.labs.shaders 1.0
import "../../lib/components"
Item{
    id:ftr
    anchors.bottom: parent.bottom
    width: qmlroot.width
    height:scaleY(10)
    onFocusChanged: if(activeFocus) scenarioList.forceActiveFocus()
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
                    if(manager.currentScreen==="Screen_1.qml"){
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
                    }else if (manager.currentScreen==="Screen_47"){

                        if(name==="Attribute")
                            console.log("attribute selected")
                    }
                    submodel.currentIndex = 0
                }

            }

            Rectangle{
                anchors.fill: parent
                radius:5
                color: currentItem===index ? appStyle.lightHighlightColor : appStyle.darkHighlightColor
            }
            Text{
                anchors.centerIn: parent
                text:name
                font.pixelSize: 32
            }
            Keys.onTabPressed: swapFocus()
            Keys.onUpPressed:  submodel.decrementCurrentIndex()
            Keys.onDownPressed: submodel.incrementCurrentIndex()

            ListView{
                id:submodel
                height:ftr.currentItem === index ?childrenRect.height : 0
                width: parent.width
                focus: true

              onHeightChanged: {positionViewAtEnd(); console.log(name+" expanding")}

                orientation:ListView.Vertical
                clip:true
                anchors.bottom: parent.top
                spacing:scaleY(1)
                delegate: Rectangle{
                    radius:5
                    clip: true
                    color:submodel.currentIndex === index ? "grey" : "black"
                    height:label.paintedHeight
                    width: parent.width
                    opacity: .65
                    Text {
                        id:label
                        color:"white"
                        text: title
                        width:parent.width
                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                        anchors.centerIn: parent
                        font.pointSize: 28

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

    Keys.onTabPressed:swapFocus()
    onActiveFocusChanged: {
        if(activeFocus)
            console.log("Footer gained focus")
        else
            console.log("Footer lost focus")
    }
}
