import QtQuick 2.2
import "."
Item{
    id:newOrbiterButton
    height:0
    anchors{
        left:parent.left
        right:parent.right
        bottom:parent.bottom
    }
    state:"hidden"
    Component.onCompleted: state="showing"


    Rectangle{
        opacity: .25
        color:"black"
        anchors.fill: parent
    }

    Text {
        id: newOrbiterLabel
        text: qsTr("Would you like to create a new orbiter?", "Create a new orbiter.")
        font.pixelSize: theme.appFontSize_title
        anchors.centerIn: parent
        width: parent.width*.85
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        color:"white"
    }

    Image{
        id:click_icn
        source:"images/vertical_arrow.png"
        rotation:-90
        height: parent.height /2
        fillMode: Image.PreserveAspectFit
        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }
    }
    
    MouseArea{
        anchors.fill: parent
        onClicked:screenchange("SetupNewOrbiter.qml")
    }

    states: [
        State {
            name: "hidden"
            when:!orbiterWindow.b_connectionPresent
            PropertyChanges {
                target: newOrbiterButton
                opacity:0
                height:0
            }

        },
        State {
            name: "showing"
            when:orbiterWindow.b_connectionPresent
            PropertyChanges {
                target: newOrbiterButton
                height:theme.appNavigation_panelHeight
                opacity:1
            }

        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            PropertyAnimation{
                target: newOrbiterButton
                property: "height"
                duration: 350
            }
        }
    ]


}
