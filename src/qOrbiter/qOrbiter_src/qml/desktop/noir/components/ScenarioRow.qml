// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1


Rectangle {
    id:scenarios
    width: appW *.85
    height: modelRow.height+presenceRow.height
   // color: "grey"
    radius: 5
    gradient: style.generalGradient

    anchors.horizontalCenter: parent.horizontalCenter


    Row{
        id:modelRow
        height: scenarios.height
        width: scenarios.width
        spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle{
            id:lightsTect
            height: 50
            width: 85
            color: "transparent"
            Text {
                id: lLabel
                text: qsTr("Lights")
                anchors.centerIn: parent
                color: "white"
            }
        }

        Rectangle{
            id:mediaRect
            height: 50
            width: 85
            color: "transparent"
            Text {
                id: mLabel
                text: qsTr("Media")
                anchors.centerIn: parent
                color: "white"
            }
        }

        Rectangle{
            id:climateRect
            height: 50
            width: 85
            color: "transparent"
            Text {
                id: cLabel
                text: qsTr("Climate")
                anchors.centerIn: parent
                color: "white"
            }
        }

        Rectangle{
            id:securityRect
            height: 50
            width: 85
            color: "transparent"
            Text {
                id: sLabel
                text: qsTr("Security")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
            }
        }

        Rectangle{
            id:telecomRect
            height: 50
            width: 85
            color: "transparent"
            Text {
                id: tLabel
                text: qsTr("Telecom")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
            }
        }

        Rectangle{
            id:advanced
            height: 50
            width: 85
            color: "transparent"
            Text {
                id: aLabel
                text: qsTr("Advanced")
                anchors.centerIn: parent
                color: "white"
                font.family: "Nimbus Sans L"
            }
        }
    }
    //user and location info

    Row{
        id:presenceRow
        height: 50
        width: scenarios.width
        anchors.top: modelRow.bottom
        anchors.left: modelRow.left
        spacing: 5

        Rectangle{
            id:roomRect
            height: 50
            width: 85
            color: "transparent"
            Text {
                id: roomLabel
                text: qsTr("room")
                font.pixelSize: 12
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
