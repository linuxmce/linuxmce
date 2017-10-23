import QtQuick 2.2
import "../components"
import "../."
StyledScreen{
    id:ps3_harmony_ir

    Panel{
        id:ps3Panel
        state:"gamepad"
        headerTitle: "Playstation " + ps3Panel.state.toString()
        anchors.fill: parent
        buttonContent:[
            StyledButton{
                buttonText: qsTr("Gamepad", "Gamepad remote")
                onActivated: panelContent.state="gamepad"
                height:parent.height

            },
            StyledButton{
                buttonText: qsTr("Remote", "Video Game Remote")
                onActivated: panelContent.state="remote"
                height:parent.height
            },
            StyledButton{
                buttonText: qsTr("Options", "Media Options")
                onActivated: panelContent.state="remote"
                height:parent.height
            },
            StyledButton{
                buttonText: qsTr("Power", "Stop Media")
                onActivated: manager.stopMedia()
                height:parent.height
            }
        ]

        content: Item{
            id:panelContent
            anchors.fill: parent

            Item{
                id:gamePadPanel
                anchors{
                    top:parent.top
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
                    width: appStyle.appButtonWidth/2

                    StyledButton{
                        width: parent.width
                        buttonText:"L3"
                        onActivated: manager.extraButtonPressed(buttonText)
                        height:parent.height/3
                        txtObj.font.family: "1979"
                        txtObj.font.bold: true
                    }
                    StyledButton{
                        width: parent.width
                        height:parent.height/3
                        buttonText:"L2"
                        txtObj.font.bold: true
                        txtObj.font.family: "1979"
                        onActivated: manager.extraButtonPressed(buttonText)
                    }
                    StyledButton{
                        width: parent.width
                        height:parent.height/3
                        buttonText:"L1"
                        txtObj.font.bold: true
                        txtObj.font.family: "1979"
                        onActivated: manager.extraButtonPressed(buttonText)
                    }
                }
            }

            Column{
                id:centerCol
                anchors{
                    top:parent.top
                    horizontalCenter: parent.horizontalCenter
                }
                width: parent.width *.35
                height: parent.height*.15
                Row{
                    id:center_btns
                    width: parent.width
                    height: parent.height/2
                    anchors.horizontalCenter: parent.horizontalCenter
                    StyledButton{
                        width: parent.width/2
                        height: parent.height
                        buttonText: qsTr("Select", "Game Controller Select Button")
                        onActivated: manager.extraButtonPressed("select")
                        txtObj.font.bold: true
                        txtObj.font.family: "1979"
                    }
                    StyledButton{
                        width: parent.width/2
                        height: parent.height
                        buttonText: qsTr("Start", "Game Controller Start Button")
                        onActivated: manager.EnterGo()
                        txtObj.font.bold: true
                        txtObj.font.family: "1979"
                    }

                }
                StyledButton{
                    width: parent.width
                    height: parent.height/2
                    buttonText: "PS"
                    onActivated: manager.extraButtonPressed("psbutton")
                    txtObj.font.bold: true
                    txtObj.font.family: "1979"
                }

            }


            Column{
                id:rButtons
                anchors{
                    top:parent.top
                    right:parent.right
                }
                height: parent.height *.25
                width: appStyle.appButtonWidth/2
                StyledButton{
                    width: parent.width
                    height: parent.height/3
                    buttonText:"R3"
                    onActivated: manager.extraButtonPressed(buttonText)
                    txtObj.font.bold: true
                    txtObj.font.family: "1979"
                }
                StyledButton{
                    width: parent.width
                    height: parent.height/3
                    buttonText:"R2"
                    onActivated: manager.extraButtonPressed(buttonText)
                    txtObj.font.bold: true
                    txtObj.font.family: "1979"
                }
                StyledButton{
                    width: parent.width
                    height: parent.height/3
                    buttonText:"R1"
                    onActivated: manager.extraButtonPressed(buttonText)
                    txtObj.font.bold: true
                    txtObj.font.family: "1979"
                }
            }

            DirectionPad{
                visible:true
                anchors{
                    left:parent.left
                    bottom:parent.bottom
                }
            }

            PSMediaSquare {
                id: mediaSquare
                anchors{
                    bottom:parent.bottom
                    right:parent.right
                    rightMargin: width*.20
                    bottomMargin: width*.20
                }
            }

            Item{
                id:numberEntry
                //visible: !manager.isProfile
                anchors{
                    top:centerCol.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                height: childrenRect.height
                width: manager.isProfile ? parent.width *.50 : parent.width *.30
                Flow {
                    id: flow1
                    anchors.top:parent.top
                    width: parent.width
                    height: childrenRect.height

                    Repeater{
                        model: 9
                        StyledButton {
                            state:"numberpad-small"
                            buttonText: (index+1)
                            txtObj.font.bold: true
                            txtObj.font.family: "1979"
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
                        txtObj.font.bold: true
                        txtObj.font.family: "1979"

                    }
                    StyledButton {
                        state:"numberpad-small"
                        buttonText: "0"
                        txtObj.font.bold: true
                        txtObj.font.family: "1979"

                    }
                    StyledButton {
                        state:"numberpad-small"
                        buttonText: "back"
                        txtObj.font.bold: true
                        txtObj.font.family: "1979"
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
                    //target: gamePadPanel
                    // visible:true
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




