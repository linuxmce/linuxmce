// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
import QtQuick 2.0
import "../js/ComponentLoader.js" as MyJs

Rectangle {
    id:scenarios
    width: manager.appWidth
    height:manager.appHeight *.10
    color: activeFocus ? "slategrey" : "transparent"
    radius: 5
    focus:false
   // Keys.onTabPressed: {shiftFocus(); console.log("Tab to next focus scope?") }
    onActiveFocusChanged: console.log("Scenario Row focus::"+focus)
    gradient: style.generalGradient
    property bool currentState:true
    property double buttonWidth:150
    property double buttonHeight:50
    property double scenarioFontSize:manager.appHeight * .03
    state:"show"
    states: [
        State {
            name: "show"
            AnchorChanges {
                target: scenarios
              //  anchors.bottom: undefined
                anchors.bottom: parent.bottom

            }
        },
        State {
            name: "hide"
            AnchorChanges {
                target:scenarios
              //  anchors.top: undefined
                anchors.top: parent.bottom
            }
        }
    ]
    transitions: [
        Transition {
            from: "*"
            to: "*"
            AnchorAnimation{
                duration: 750
                targets: scenarios
            }
        }
    ]

    Rectangle{
        id:transparency
        anchors.fill: parent
        color: "black"
        opacity: .25
    }

    FocusScope{
        width: scenarios.width
        height:scenarios.height
        property alias color: scenarios.color
        Keys.onTabPressed: {shiftFocus(); console.log("Shifting back to controls")}

        onFocusChanged: console.log("Scenarios have focus")

    Row{
        id:modelRow
        height:parent.width
        width: buttonWidth*6
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle{
            id:lightsTect
            height: buttonHeight
            width: buttonWidth
            property bool popEnabled: false
            color: "transparent"
            StyledText {
                id: lLabel
                text: qsTr("Lights")
                anchors.centerIn: parent
                color: "white"
                font.pointSize: scenarioFontSize
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

        Rectangle{
            id:mediaRect
            height: buttonHeight
            width: buttonWidth
            color: "transparent"
            property bool popEnabled:false
            StyledText {
                id: mLabel
                text: qsTr("Media")
                anchors.centerIn: parent
                color: "white"
                font.pointSize: scenarioFontSize
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    if(!parent.popEnabled)
                    {

                    MyJs.createScenarioBox(currentRoomMedia, 100, 400, parent )
                    }
                }
            }
        }

        Rectangle{
            id:climateRect
            height: buttonHeight
            width: buttonWidth
            color: "transparent"
            property bool popEnabled:false
            StyledText {
                id: cLabel
                text: qsTr("Climate")
                anchors.centerIn: parent
                color: "white"
                font.pointSize: scenarioFontSize
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    if(!parent.popEnabled)
                    {

                    MyJs.createScenarioBox(currentRoomClimate, 100, 400, parent )
                    }
                }
            }
        }

        Rectangle{
            id:securityRect
            height: buttonHeight
            width: buttonWidth
            color: "transparent"
            property bool popEnabled:false
            StyledText {
                id: sLabel
                text: qsTr("Security")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
                font.pointSize: scenarioFontSize
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    if(!parent.popEnabled)
                    {

                    MyJs.createScenarioBox(currentRoomSecurity, 100, 400, parent )
                    }
                }
            }
        }

        Rectangle{
            id:telecomRect
            height: buttonHeight
            width: buttonWidth
            color: "transparent"
            property bool popEnabled:false
            StyledText {
                id: tLabel
                text: qsTr("Telecom")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
                font.pointSize: scenarioFontSize
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    if(!parent.popEnabled)
                    {

                    MyJs.createScenarioBox(currentRoomTelecom, 100, 400, parent )
                    }
                }
            }
        }

        Rectangle{
            id:advanced
            height: buttonHeight
            width: buttonWidth
            color: "transparent"
            StyledText {
                id: aLabel
                text: qsTr("Advanced")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
                font.pointSize: scenarioFontSize
            }
        }
    }
    //user and location info

    Row{
        id:presenceRow
        height: 100
        width: modelRow.width

        spacing: 5

        Rectangle{
            id:roomRect
            height: 50
            width: 85
            color: "transparent"
            StyledText {
                id: roomLabel
                text: qsTr("room")

                anchors.left: parent.left
                color: "white"
                font.family: "Nimbus Sans L"
            }

            StyledText {
                id: roomTitle
                text: currentroom
                font.pointSize: 18
                anchors.left: roomLabel.right
                color: "white"
                font.family: "Nimbus Sans L"
            }
        }

        Rectangle{
            id:userRect
            height: 50
            width: 85
            color: "transparent"
            StyledText {
                id: userLabel
                text: qsTr("user:")
                font.pointSize: 12
                anchors.left: userRect.left
                color: "white"
                font.family: "Nimbus Sans L"
            }
            StyledText {
                id: userName
                text: currentuser
                font.pointSize: 18
                anchors.left: userLabel.right
                color: "white"
                font.family: "Nimbus Sans L"
            }
        }
    }

}
}
