import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs


Rectangle{
    id:scenarioTrigger
    height: buttonHeight
    width: buttonWidth
    color: "transparent"
    property variant targetModel
    property string triggerLabel:"Ipsum"
    focus:true
    Rectangle{
        id:angle
        rotation: 45
        width: buttonHeight *.30
        height: buttonHeight*.30
        color: style.highlight1
        opacity: .25
       // border.color: "white"
       // border.width: 1
        visible: targetModel === popupHolder.scenarioModel ? true : false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.top

    }

    StyledText {
        id: scenario_trigger_label
        text: triggerLabel
        anchors.centerIn: parent
        color:"white"
        font.pointSize: scaleY(3)
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {          
            popupHolder.scenarioModel = targetModel;
            popupHolder.popEnabled = true
        }
    }
}
