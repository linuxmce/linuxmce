import QtQuick 2.2

ListView{
    id:existing_orbiters
    height: scaleY(55)
    width: scaleX(75)
    clip: true
    anchors.left: parent.right
    anchors.verticalCenter: parent.verticalCenter
    model:orbiterList   
    spacing: scaleY(1)
    opacity: 0
    Behavior on opacity{
        PropertyAnimation{
            duration: 1500
        }
    }

    delegate: Rectangle{
        id:existing_orbiter_delegate
        height: labelColumn.height
        width: existing_orbiters.width

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
        border.color: "#99CC99"
        border.width: 1
        Column
        {
            id:labelColumn
            height: childrenRect.height
            width: parent.width
            anchors.centerIn: parent
            
            Text {
                id: orbiter_label
                text: qsTr("Orbiter:")+ label
                font.pixelSize: infoTextSize
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.family: myFont.name

                width: parent.width
                color: "#99CC99"
            }
//            Text {
//                id: dev_num
//                text:qsTr("Device:")+ device
//                font.pixelSize:  infoTextSize
//                font.italic: true
//                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
//                font.family: myFont.name
//                width: parent.width
//            }
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
