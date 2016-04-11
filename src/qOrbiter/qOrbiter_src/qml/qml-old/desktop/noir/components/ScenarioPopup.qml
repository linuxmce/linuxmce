// import QtQuick 1.1 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id:scenarioPopup
    property variant scenarioModel
    property int componentHeight
    property int componentWidth
    property string scenarioName
    property color bgColor :"transparent"
    property color delegateActiveColor:"grey"
    property color delegatePressedColor: "darkgrey"
    property color delegateHoverColor: "lightgrey"
    property bool popEnabled:false
    z:10
    focus: true
    color: bgColor
    onPopEnabledChanged: popEnabled ? showScenarios():""

    function showScenarios(){
        focus = true
        forceActiveFocus()
        scenarioView.visible = true
        z = 5
    }

    width: componentWidth
    height: componentHeight
    Timer{
        id:closeTimer
        interval: 750
        triggeredOnStart: false
        running: true
        onTriggered: {
            popEnabled = false
            scenarioModel=""
        }
    }
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onEntered: popEnabled = true
        onExited: closeTimer.start()
    }

    HorizontalScenarios{
        id:scenarioView
        anchors.bottom: scenarioPopup.top
        anchors.bottomMargin: buttonHeight*.20
        visible: popEnabled ? true : false
    }
}
