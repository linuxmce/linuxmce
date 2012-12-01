// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:scenarios
    width: parent.width
    height:manager.appHeight *.10
    color: "transparent"
    radius: 5
    Image {
        id: rowBg
        source: "../img/thin_bar.png"
        anchors.fill: parent
    }
    property double buttonWidth:150
    property double buttonHeight:50
    property double scenarioFontSize:manager.appHeight * .03
    anchors.horizontalCenter: parent.horizontalCenter

    ScenarioPopup{
        id:popupHolder
        scenarioModel: currentRoomLights
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
            Text {
                id: aLabel
                text: qsTr("Advanced")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
                font.pixelSize: scenarioFontSize
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
            Text {
                id: roomLabel
                text: qsTr("room")
                anchors.left: parent.left
                color: "white"
                font.family: "Nimbus Sans L"
            }

            Text {
                id: roomTitle
                text: currentroom
                font.pixelSize: 18
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

            Text {
                id: userLabel
                text: qsTr("user:")
                font.pixelSize: 12
                anchors.left: userRect.left
                color: "white"
                font.family: "Nimbus Sans L"
            }
            Text {
                id: userName
                text: currentuser
                font.pixelSize: 18
                anchors.left: userLabel.right
                color: "white"
                font.family: "Nimbus Sans L"
            }
        }
    }

}
