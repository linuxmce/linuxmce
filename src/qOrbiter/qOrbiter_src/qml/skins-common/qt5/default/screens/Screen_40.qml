import QtQuick 2.2
import "../components"
StyledScreen{
    id:ps3_harmony_ir

    Panel{
        id:ps3Panel
        state:"gamepad"
        headerTitle: "Playstation " + ps3Panel.state.toString()
        anchors.fill: parent
        Row{
            parent:ps3Panel.headerRect
            anchors{
                left:parent.horizontalCenter
                right:parent.right
                bottom:parent.bottom
                top:parent.top
            }

            StyledButton{
                buttonText: "gamepad"
                onActivated: ps3Panel.state="gamepad"
            }
            StyledButton{
                buttonText: "remote"
                onActivated: ps3Panel.state="remote"
            }
        }

        Item{
            id:gamePadPanel
            anchors{
                top:ps3Panel.headerRect.bottom
                left:parent.left
                right:parent.right
                bottom: parent.bottom
            }

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
            PSMediaSquare {
                id: mediaSquare
                anchors{
                    verticalCenter: parent.verticalCenter
                    verticalCenterOffset: scaleY(8)
                    left:parent.left
                }
            }

            Item{
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: scaleY(8)
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

        Item{
            id:remotePanel
            anchors.fill: parent
        }

        states: [
            State {
                name: "gamepad"
                PropertyChanges {
                    target: remotePanel
                    visible:false
                }
                PropertyChanges {
                    target: gamePadPanel
                    visible:true
                }
            },
            State {
                name: "remote"
                PropertyChanges {
                    target: gamePadPanel
                    visible:false
                }
                PropertyChanges {
                    target: remotePanel
                    visible:true
                }
            }
        ]

    }



}


