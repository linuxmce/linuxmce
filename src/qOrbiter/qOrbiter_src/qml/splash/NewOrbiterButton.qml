import QtQuick 2.3
import "."
Item{
    id:newOrbiterButton
    height:0
    width:scaleX(100)

    anchors.bottom: parent.bottom
    Component.onCompleted: state="hidden"
    anchors.horizontalCenter: parent.horizontalCenter

    Rectangle{
        radius:10
        color:Style.appbutton_confirm_color
        anchors.fill: parent
    }


        Text {
        id: newOrbiterLabel
        text: qsTr("Would you like to create a new orbiter?", "Create a new orbiter.")
        font.pointSize: Style.appFontSize_title
        anchors.centerIn: parent
        width: parent.width*.75
        anchors.verticalCenter: parent.verticalCenter
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere      
        color:"white"
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
                height:0
            }

        },
        State {
            name: "showing"
            PropertyChanges {
                target: newOrbiterButton

                anchors.leftMargin: scaleX(25)
                height:Style.appButtonHeight
            }

        }
    ]

    transitions: Transition {
        // smoothly reanchor myRect and move into new position
        PropertyAnimation{
            duration: 1000
        }
    }
}
