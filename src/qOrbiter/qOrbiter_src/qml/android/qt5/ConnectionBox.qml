import QtQuick 2.2
Item{
    id: connectionBoxContainer
    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenterOffset:-25
    width: manager.appWidth *.75
    height: scaleY(20)
    state:"hidden"

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
        font.pointSize: 16        
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter       
    }

    Row{
        id:connectionVars
            anchors.centerIn: parent
            width:childrenRect.width
            height: childrenRect.height
            spacing: scaleX(2)

            Text {
                text: qsTr("Host:")
                font.pixelSize: 24
                font.family: "Roboto-Regular"
                font.bold: false
            }

            TextInput {
                id: routerip

                text: orbiterWindow.router
                font.pixelSize:24
                font.family:"Roboto"
            }

            TextInput {
                id: ext_routerip
                width: 80
                text: extip
                font.pixelSize: 24
                font.family:"Roboto"
                visible: false
            }

            Text {
                text: qsTr("Device:")
                font.pixelSize: 24
                font.family: "Roboto"
            }
            TextInput {
                id: devicenumber
                width: scaleX(10)
                text: orbiterWindow.deviceno
                font.family: "Roboto"
                font.pixelSize:24
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
            height: scaleY(3)
            width: scaleX(12)
            color:"transparent"
            border.color: "black"
            border.width: 1
            radius:5
            Text {
                id: name
                anchors.centerIn: parent                
                text: qsTr("Go!")
                font.pointSize: 20
                verticalAlignment: Text.AlignTop
                font.family:"Roboto"
            }

            MouseArea{
                hoverEnabled: true
                onEntered: parent.color="green"
                onExited: parent.color="red"
                anchors.fill: parent
                onClicked:orbiterWindow.qmlSetupLmce(devicenumber.text, routerip.text)
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Item {
            id: exitbutton
            height: scaleY(3)
            width: scaleX(12)

            Rectangle{
                anchors.fill: parent
                color:"transparent"
                border.color: "black"
                border.width: 1
                 radius:  5
            }

            Text {
                id: exitlabel
                anchors.centerIn: parent      
                text: qsTr("Exit")
                font.pixelSize: 20
                font.family: "Roboto"
            }

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
                target: connectionBoxContainer
                opacity:1
                scale:1
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: connectionBoxContainer
                opacity:0
                scale:0
            }
        }
    ]
}
