import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs


Rectangle{
    id:lightsTect
    height: buttonHeight
    width: buttonWidth

    property bool popEnabled: false
    color: "transparent"
    Text {
        id: lLabel
        text: qsTr("Lights")
        anchors.centerIn: parent
        color: "white"
        font.pixelSize: scenarioFontSize
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            if(!parent.popEnabled)
            {
                
                MyJs.createScenarioBox(currentRoomLights, 100, 400, parent )
            }
        }
    }
}
