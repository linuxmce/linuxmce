import QtQuick 2.2
Item{
    id: connectionBoxContainer
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenterOffset:-10
    width: parent.width -10
    height:  scaleY(35)
    Rectangle {
        anchors.fill: parent
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
        font.pointSize: infoTextSize
        font.bold: true
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: myFont.name
        color:"white"
    }

    Row{
        id:connectionVars
        anchors.top: connectionlabel.bottom
        spacing: 10
        height: childrenRect.height
        width: childrenRect.width

        Column{
            height: parent.height
            width: childrenRect.width
            spacing:scaleY(1)
            Text {
                text: qsTr("Host:")
                font.pointSize:infoTextSize
                font.family: myFont.name
                color:"white"
            }

            Text {
                text: qsTr("Device:")
                font.pointSize: infoTextSize
                font.family: myFont.name
                color:"white"
            }

        }


        Column{
            height: parent.height
            width: childrenRect.width
            spacing:scaleY(1)

            TextInput {
                id: routerip
                width: scaleX(40)
                text:orbiterWindow.router
                font.pointSize: infoTextSize
                font.family: myFont.name
                //  onTextChanged: setRouterIp(routerip.text)

                Keys.onReleased: {
                    switch(event.key){
                    case Qt.Key_Enter:
                        closeSoftwareInputPanel()
                        break;
                    case Qt.Key_Return:
                        closeSoftwareInputPanel()
                        break;
                    default:
                        console.log("Caught Key");
                        console.log(event.key)
                        break
                    }
                    event.accepted = true
                }

            }

            TextInput {
                id: devicenumber
                width: scaleX(10)
                text: orbiterWindow.deviceno
                font.family: myFont.name
                font.pointSize: infoTextSize
                //onTextChanged: setDeviceNo(devicenumber.text)

                Keys.onReleased: {
                    switch(event.key){
                    case Qt.Key_Enter:
                        closeSoftwareInputPanel()
                    }
                }
            }

            TextInput {
                id: ext_routerip
                width: 80
                height: visible? devicenumber.height :0
                text: extip
                font.pointSize:infoTextSize
                font.family: myFont.name
                //  onTextChanged: setRouterIp(routerip.text)

                visible: false
            }
        }

    }

    Row{
        id:goBox
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        spacing: scaleX(15)
        Rectangle {
            id: connectbutton
            height: startButtonHeight
            width: startButtonWidth
            color:"red"

            Text {
                id: name
                anchors.centerIn: parent

                text: qsTr("Go!")
                font.pointSize: infoTextSize
                verticalAlignment: Text.AlignTop
                font.bold: true
                font.family: myFont.name
                color:"white"
            }

            radius:  5
            MouseArea{
                hoverEnabled: true
                onEntered: parent.color="green"
                onExited: parent.color="red"
                anchors.fill: parent
                onClicked:orbiterWindow.qmlSetupLmce(devicenumber.text, routerip.text)
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Rectangle {
            id: exitbutton
            height: startButtonHeight
            width:startButtonWidth
            Text {
                id: exitlabel
                anchors.centerIn: parent
                text: qsTr("Exit")
                font.pixelSize:infoTextSize
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
