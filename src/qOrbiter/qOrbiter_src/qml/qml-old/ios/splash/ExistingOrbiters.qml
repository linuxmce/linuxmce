import QtQuick 2.2

ListView{
    id:existing_orbiters
    height: scaleY(55)
    width: scaleX(100)
    clip: true
    anchors.left: parent.right
    anchors.verticalCenter: parent.verticalCenter
    model:orbiterList
    orientation: ListView.Horizontal

    visible: true
    spacing: scaleY(1)
    opacity: 0
    Behavior on opacity{
        PropertyAnimation{
            duration: 1500
        }
    }

    delegate: Rectangle{
        id:existing_orbiter_delegate
        height: label !=undefined ?  250 : 0
        width: height
        radius: width
        color:"green"

        border.color: "#99CC99"
        border.width: 1
        Text {
            id: dev_num
            text: device
            opacity: .25
            font.pixelSize: 125
            font.italic: true
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.family: myFont.name
            anchors.centerIn: parent
        }

            Text {
                id: orbiter_label
                text: qsTr("Orbiter:")+ label
                font.pixelSize: infoTextSize
                font.weight: Font.Light
                anchors.centerIn: parent
                anchors.horizontalCenterOffset: 25
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
                color: "#99CC99"
            }


        MouseArea {
            anchors.fill: parent
            onClicked: orbiterWindow.qmlSetupLmce(device, manager.usingExternal ? manager.externalHost : manager.internalHost)
        }
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: existing_orbiters
                opacity:0
            }
        },
        State {
            name: "showing"
            PropertyChanges {
                target: existing_orbiters
                opacity:1
                anchors.rightMargin: 10
            }
            AnchorChanges{
                target: existing_orbiters
                anchors.left: undefined
                anchors.right: parent.right
            }
        }
    ]
}
