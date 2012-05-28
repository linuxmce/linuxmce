// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:scenarioPopup
    property variant scenarioModel
    property int componentHeight
    property int componentWidth
    property color bgColor :"transparent"
    property color delegateActiveColor:"grey"
    property color delegatePressedColor: "darkgrey"
    property color delegateHoverColor: "lightgrey"
    color: bgColor
    Component.onCompleted: parent.popEnabled = true

    Timer{
        id:closeTimer
        interval: 2500
        triggeredOnStart: false
        running: true
        onTriggered: {
            scenarioPopup.parent.popEnabled = false;
            scenarioPopup.destroy()
        }
    }
    width: componentWidth
    height: componentHeight
    anchors.bottom: parent.top

    ListView{
        id:scenarioList
        height: componentHeight
        width: componentWidth

        anchors.centerIn: scenarioPopup
        model: scenarioModel
        delegate: Rectangle{
            height: 50
            width: componentWidth
            color: delegateHit.containsMouse ? delegateHit.pressed ? delegatePressedColor : delegateHoverColor : delegateActiveColor
            Text {
                id: scenarioItem
                text: title
                font.pixelSize: 12
                anchors.centerIn: parent
            }
            MouseArea{
                anchors.fill: parent
                id:delegateHit
                hoverEnabled: true
                onClicked:dcerouter.executeCommandGroup(params);
                onExited: closeTimer.start()
                onEntered: closeTimer.stop()
            }
        }


    }

}
