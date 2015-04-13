import QtQuick 2.2

Rectangle{
    id:newOrbiterButton
    height: scaleY(10)
    width:scaleX(55)
    opacity: 0
    border.color: "#99CC99"
    anchors.verticalCenter: parent.verticalCenter
    Component.onCompleted: state="hidden"
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
                anchors.right: parent.left
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
                anchors.top: undefined
                anchors.right: undefined
                anchors.horizontalCenter: splashPage.horizontalCenter
                anchors.verticalCenter: splashPage.verticalCenter
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
