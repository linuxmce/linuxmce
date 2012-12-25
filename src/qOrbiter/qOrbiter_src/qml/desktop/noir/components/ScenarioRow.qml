// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs
//FocusScope{
//    height: scenarios.height
//    width: scenarios.width


//    onFocusChanged: console.log("Focus for Scenario Container is now "+focus)
//    onActiveFocusChanged: console.log("ActiveFocus for Scenario Container is now " +activeFocus)

Rectangle {
    id:scenarios
    width: manager.appWidth
    height:manager.appHeight *.10
    color: "transparent"
    border.color: "red"

    property double buttonWidth:150
    property double buttonHeight:50
    property double scenarioFontSize:manager.appHeight * .03

    radius: 5
    focus:true
    // Keys.onTabPressed: lmceScenarios.focus= !lmceScenarios.focus
    onActiveFocusChanged: activeFocus ? "Scenario Row has active focus" : "Scenario Row lost active focus"
    onFocusChanged: console.log("Focus inner container is now "+ focus)

    Image {
        id: rowBg
        source: "../img/thin_bar.png"
        anchors.fill: parent
    }

    anchors.horizontalCenter: parent.horizontalCenter

    ScenarioPopup{
        id:popupHolder
        scenarioModel: ""
        componentHeight: buttonHeight
        componentWidth: buttonWidth
        scenarioName: "Lighting"
    }

    Row{
        id:modelRow
        height: scenarios.height
        width: buttonWidth*6
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        ScenarioTrigger {
            id: lightingTrigger
            triggerLabel: "Lights"
            targetModel: currentRoomLights
        }

        ScenarioTrigger{
            id:mediaTrigger
            triggerLabel: "Media"
            targetModel: currentRoomMedia
        }

        ScenarioTrigger{
            id:climateTrigger
            triggerLabel: "Climate"
            targetModel: currentRoomClimate
        }

        ScenarioTrigger{
            id:securityTrigger
            triggerLabel:"Security"
            targetModel: currentRoomSecurity
        }

        ScenarioTrigger{
            id:telecomTrigger
            triggerLabel: "Telecom"
            targetModel: currentRoomTelecom
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
                font.pixelSize: scenarioFontSize
            }
            MouseArea{
                anchors.fill: parent
                onClicked: manager.gotoQScreen("Screen_44.qml")
            }
        }
    }
    //user and location info

    Row{
        id:presenceRow
        height: childrenRect.height
        width: manager.appWidth
        anchors.top: modelRow.bottom
        anchors.horizontalCenter: parent.horizontalCenter
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

            }

            StyledText {
                id: roomTitle
                text: manager.getCurrentRoom()
                font.pixelSize: 18
                anchors.left: roomLabel.right
                color: "white"

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
                font.pixelSize: 12
                anchors.left: userRect.left
                color: "white"

            }
            StyledText {
                id: userName
                text: manager.getCurrentUser()
                font.pixelSize: 18
                anchors.left: userLabel.right
                color: "white"
            }
        }
    }
    states: [
        State {
            name: "focused"
            when:scenarios.focus
            PropertyChanges {
                target: scenarios
                border.width: 2
            }
        },
        State{
            name:"unfocused"
            when:!scenarios.focus
            PropertyChanges {
                target: scenarios
                border.width: 0

            }
        }

    ]
}


