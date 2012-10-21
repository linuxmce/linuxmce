// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../js/ComponentLoader.js" as MyJs


Rectangle {
    id:scenarios
    width: appW
    height:appH *.10
   // color: "grey"
    radius: 5
    gradient: style.generalGradient
    property double buttonWidth:150
    property double buttonHeight:50
    property double scenarioFontSize:appH * .03
    anchors.horizontalCenter: parent.horizontalCenter

    Row{
        id:modelRow
        height: scenarios.height
        width: buttonWidth*6
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

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

        Rectangle{
            id:mediaRect
            height: buttonHeight
            width: buttonWidth
            color: "transparent"
            property bool popEnabled:false
            Text {
                id: mLabel
                text: qsTr("Media")
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
            Text {
                id: cLabel
                text: qsTr("Climate")
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
            Text {
                id: sLabel
                text: qsTr("Security")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
                font.pixelSize: scenarioFontSize
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
            Text {
                id: tLabel
                text: qsTr("Telecom")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
                font.pixelSize: scenarioFontSize
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
        height: 100
        width: modelRow.width
        anchors.bottom: scenarios.bottom
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
