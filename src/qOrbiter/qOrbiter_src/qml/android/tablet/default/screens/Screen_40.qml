import QtQuick 1.1
import "../components"
StyledScreen{
    id:ps3_harmony_ir


    Column{
        id:lButtons
        anchors{
            top:parent.top
            left:parent.left
        }
        height: parent.height *.25
        StyledButton{

            buttonText:"L3"
            onActivated: manager.extraButtonPressed(buttonText)
        }
        StyledButton{

            buttonText:"L2"
            onActivated: manager.extraButtonPressed(buttonText)
        }
        StyledButton{

            buttonText:"L1"
            onActivated: manager.extraButtonPressed(buttonText)
        }
    }

    Row{
        id:center_btns
        width: parent.width *.25
        height: childrenRect.height
        anchors.horizontalCenter: parent.horizontalCenter
        StyledButton{
            buttonText: "Select"
            onActivated: manager.extraButtonPressed("select")
        }
        StyledButton{
            buttonText: "start"
            onActivated: manager.EnterGo()
        }

    }
    StyledButton{
        anchors{
            left:center_btns.left
            right:center_btns.right
            top:center_btns.bottom
        }
        buttonText: "PS"
        onActivated: manager.extraButtonPressed("psbutton")
    }

    Column{
        id:rButtons
        anchors{
            top:parent.top
            right:parent.right
        }
        height: parent.height *.25
        StyledButton{
            buttonText:"R3"
            onActivated: manager.extraButtonPressed(buttonText)
        }
        StyledButton{
            buttonText:"R2"
            onActivated: manager.extraButtonPressed(buttonText)
        }
        StyledButton{
            buttonText:"R1"
            onActivated: manager.extraButtonPressed(buttonText)
        }
    }
    Item {
        id: mediaSquare
        height: manager.isProfile ? scaleY(33) : scaleY(50)
        width:manager.isProfile ? scaleX(50) : scaleX(33)
        Rectangle{
            opacity: .25
            anchors.fill: parent
        }
        anchors{
            verticalCenter: parent.verticalCenter
            left:parent.left
        }

        StyledButton{
            id:topLeft
            state:"round"
            height: parent.height*.25
            buttonText: "triangle"
            anchors{
                left:parent.left
                top:parent.top
            }
            onActivated: manager.extraButtonPressed(buttonText)
        }

        StyledButton{
            id:topRight
            height: parent.height*.25
            state: "round"
            buttonText: "circle"
            anchors{
                right:parent.right
                top:parent.top
            }
            onActivated:manager.extraButtonPressed(buttonText)
        }

        StyledButton{
            id:bottomLeft
            height: 75
            state: "round"
            buttonText: "square"
            anchors{
                left:parent.left
                bottom:parent.bottom
            }
            onActivated:manager.extraButtonPressed(buttonText)
        }

        StyledButton{
            id:bottomRight
            height: parent.height*.25
            state: "round"
            buttonText: "x"
            anchors{
                right:parent.right
                bottom:parent.bottom
            }
            onActivated: manager.extraButtonPressed(buttonText)
        }

        ImageButton {
            id: up
            height: 100
            width: 100
            upImage: "../images/arrow.png"
            downImage: "../images/arrow.png"
            onActivated: manager.moveDirection(1)
            anchors.horizontalCenter: centerBtntn.horizontalCenter
            anchors.bottom: centerBtntn.top
            imgRotation: 180
        }

        ImageButton {
            id: dn
            height: 100
            width: 100
            upImage: "../images/arrow.png"
            downImage: "../images/arrow.png"
            anchors.horizontalCenter: centerBtntn.horizontalCenter
            anchors.top: centerBtntn.bottom
            onActivated: manager.moveDirection(2)
        }
        ImageButton {
            id: left
            height: 100
            width: 100
            upImage: "../images/arrow.png"
            downImage: "../images/arrow.png"
            anchors.verticalCenter: centerBtntn.verticalCenter
            anchors.right: centerBtntn.left
            onActivated: manager.moveDirection(3)
            imgRotation: 90
        }

        ImageButton {
            id: right
            height: 100
            width: 100
            upImage: "../images/arrow.png"
            downImage: "../images/arrow.png"
            anchors.verticalCenter: centerBtntn.verticalCenter
            anchors.left: centerBtntn.right
            onActivated: manager.moveDirection(4)
            imgRotation: 270
        }



        StyledButton{
            id:centerBtntn
            height: parent.height*.40
            state: "round"
            buttonText: "Enter"
            anchors.centerIn: parent
            onActivated: manager.moveDirection(5)
        }
    }

    Item{
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: childrenRect.height
        width: parent.width *.30
        Flow {
            id: flow1
            anchors.top:parent.top
            width: parent.width
            height:3*scaleY(15)

            Repeater{
                model: 9
                StyledButton {
                    state:"numberpad-small"
                    buttonText: (index+1)
                    //onActivated: content.typeText(index+1)
                }
            }
        }
        Row{
            id:bottomRow
            height: childrenRect.height
            width:parent.width
            anchors.top: flow1.bottom

            StyledButton {
                state:"numberpad-small"
                buttonText: "<"

            }
            StyledButton {
                state:"numberpad-small"
                buttonText: "0"

            }
            StyledButton {
                state:"numberpad-small"
                buttonText: "back"
                onActivated: {

                }
            }
        }
    }

}


