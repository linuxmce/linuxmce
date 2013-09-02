import QtQuick 1.0
import "../components"
Item{
    id:sendCommandBox
    height: manager.appHeight
    width: manager.appWidth
    focus:true

    Keys.onReleased: {
        switch(event.key){
        case Qt.Key_Menu:
            phoneFloorplanLayout.state="commandView"
            event.accepted = true
            break;
        default:
            console.log("Recieved key ==>"+event.key+" but not accepting")
            break;
        }

    }

    Rectangle{
        id:phil
        anchors.fill: parent
        color:"black"
        opacity: .85
    }

    Rectangle{
        id:controlsContainer
        height: parent.height*.75
        width: parent.width*.95
        color: androidSystem.greenStandard
        radius: 5
        border.color: "white"
    }

    StyledText {
        id: cmdsPrmoptText
        text: qsTr("Availible Commands")
        fontSize: scaleY(3)
        color: "Black"
        anchors.top: controlsContainer.top
        anchors.topMargin: 10
    }
}
