import QtQuick 2.0
import "../../../../../skins-common/lib/handlers"

Item{
    id:powerControl

    Item{
        anchors{
            top:parent.top
            left:parent.left
            right:parent.right
        }
        Rectangle{
            id:hdrFill
            anchors.fill: parent
            color:style.headerBgColor
            gradient:style.bgHeaderGradient
        }
    }

    StyledText{
        anchors.bottom: btnRow.top
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Power Control")
        fontSize: scaleY(9)
    }

    Row{
        id:btnRow
        height:140
        spacing:scaleX(10)
        width: scaleX(50)
        anchors{
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }
        anchors.rightMargin: 50
        anchors.leftMargin: 50

        StyledButton{
            buttonText: qsTr("Display On")
            height: 120
            width: 200
            onActivated: manager.toggleDisplay(1)

        }

        StyledButton{
            buttonText: qsTr("Display Off")
            height: 120
            width: 200
            onActivated: manager.toggleDisplay(0)

        }
    }
    Row{
        anchors{
            top:btnRow.bottom
            left:btnRow.left
            right:btnRow.right
        }
        spacing:scaleX(10)
        height: 140
        StyledButton{
            buttonText: qsTr("Turn Off MD")
            height: 120
            width: 200
//            handler:PowerToggleHandler{mode:0}
//            useHandler: true
        }
        StyledButton{
            buttonText: qsTr("Turn On MD")
            height: 120
            width: 200
//            handler:PowerToggleHandler{mode:0}
//            useHandler: true
        }
    }

    states: [
        State {
            name: "opening"
            PropertyChanges {
                target: powerControl

            }

            AnchorChanges{
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    left:parent.left
                    right:parent.left
                }
            }
        },
        State {
            name: "ready"
            PropertyChanges {
                target: object

            }
            AnchorChanges{
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    left:parent.left
                    right:parent.right
                }
            }
        },
        State {
            name: "closing"
            PropertyChanges {
                target: powerControl
            }
            AnchorChanges{
                anchors{
                    top:parent.top
                    bottom:parent.bottom
                    left:parent.right
                    right:parent.right
                }
            }
        }
    ]
}
