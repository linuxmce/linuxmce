import QtQuick 2.2

Rectangle{
    id: connectionBox
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenterOffset:10
    width: scaleX(65)
    height: scaleY(20)
    color: "transparent"


    Rectangle {
        anchors.fill: connectionBox
        radius: 7
        anchors.horizontalCenterOffset: 1
        anchors.centerIn: parent
        border.width: 1
        border.color: "#000000"
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "transparent"
            }
            GradientStop {
                position: 1
                color: "#000000"
            }
        }
        opacity: 0.5
    }
    Text {
        id: connectionlabel
        text: qsTr("Set Connection Details")
        font.pointSize: 16
        font.bold: false
        anchors.top: connectionBox.top
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: myFont.name
    }

    Row{
        id:connectionVars
            anchors.centerIn: connectionBox
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10
            Text {
                text: qsTr("Host:")
                font.pointSize: 12
                font.family: myFont.name
                anchors.verticalCenter: parent.verticalCenter
            }

            TextInput {
                id: routerip
                width: 80
                text: orbiterWindow.router
                font.pointSize: 12
                font.family: myFont.name
                //  onTextChanged: setRouterIp(routerip.text)
                fillColor: "grey"
                anchors.verticalCenter: parent.verticalCenter

            }

            TextInput {
                id: ext_routerip
                width: 80
                text: extip
                font.pointSize: 12
                font.family: myFont.name
                //  onTextChanged: setRouterIp(routerip.text)
                fillColor: "grey"
                anchors.verticalCenter: parent.verticalCenter
                visible: false
            }

            Text {
                text: qsTr("Device:")
                font.pointSize: 12
                font.family: myFont.name
                anchors.verticalCenter: parent.verticalCenter
            }
            TextInput {
                id: devicenumber
                width: scaleX(10)
                text: orbiterWindow.deviceno
                font.family: myFont.name
                font.pointSize: 12
                //onTextChanged: setDeviceNo(devicenumber.text)
                fillColor: "grey"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

    Row{
        id:goBox
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: connectionBox.bottom
        anchors.bottomMargin: 15
        spacing: scaleX(15)
        Rectangle {
            id: connectbutton
            height: scaleY(2)
            width: scaleX(10)
            color:"red"

            Text {
                id: name
                anchors.centerIn: parent
                anchors.fill: parent
                text: qsTr("Go!")
                font.pointSize: 12
                verticalAlignment: Text.AlignTop
                font.bold: true
                font.family: myFont.name
            }

            radius:  5
            MouseArea{
                hoverEnabled: true
                onEntered: parent.color="green"
                onExited: parent.color="red"
                anchors.fill: parent
                onClicked: orbiterWindow.qmlSetupLmce(devicenumber.text, routerip.text)
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: exitbutton
            height: scaleY(3)
            width: scaleX(6)

            Text {
                id: exitlabel
                anchors.centerIn: parent
                anchors.fill: parent
                text: qsTr("Exit")
                font.pixelSize: 11
                font.family: myFont.name
            }
            radius:  4
            MouseArea{
                anchors.fill: parent
                onClicked: manager.closeOrbiter()
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
    states: [
        State {
            name: "showing"
            PropertyChanges {
                target: connectionBox
                opacity:1
                scale:1
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: connectionBox
                opacity:0
                scale:0
            }
        }
    ]
}
