import QtQuick 2.2

Rectangle{
    id:newOrbiterButton
    height: scaleY(10)
    width:scaleX(55)
    opacity: 0
    border.color: "#99CC99"
    anchors.bottom: parent.bottom
    Component.onCompleted: state="hidden"
    anchors.horizontalCenter: parent.horizontalCenter
    color:"black"
    Behavior on opacity{
        PropertyAnimation{duration: 1000}
    }

    
    Text {
        id: newOrbiterLabel
        text: qsTr("Create New Orbiter? \n Or choose and Existing Orbiter")
        font.pointSize: 15
        width: parent.width
        anchors.centerIn: parent
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.family: myFont.name
        font.bold: true
        color:"#99CC99"
    }
    
    MouseArea{
        anchors.fill: parent
        onClicked:screenchange("SetupNewOrbiter.qml")
    }

    states: [
        State {
            name: "hidden"
            PropertyChanges {
                target: newOrbiterButton
                opacity:0
            }
            AnchorChanges{
                anchors.top: parent.bottom

            }
        },
        State {
            name: "showing"
            PropertyChanges {
                target: newOrbiterButton
                opacity:1
                anchors.leftMargin: scaleX(25)
            }
            AnchorChanges{
                target: newOrbiterButton
                anchors.top: existing_orbiters.bottom

            }
        }
    ]

    transitions: Transition {
        // smoothly reanchor myRect and move into new position
        AnchorAnimation{
            duration: 1000

        }
    }
}
