import QtQuick 1.1

Item {
    id:pinorbiterWindow
    height: manager.appHeight
    width: manager.appWidth
    property int requestedUser:-1

    state:"loaded"
    MouseArea{
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {

        }
    }

    Component.onCompleted: {
        state="ready"
    }

    Connections{
        target:userList
        onPrivateUserChanged:{
            if(userList.currentPrivateUser===-1){
                inputPhil.color="red"
            }else{
                inputPhil.color="green"
                pinorbiterWindow.state="closed"
            }
        }
    }


    Rectangle{
        id:phil
        anchors{
            top:pinorbiterWindow.top
            left:pinorbiterWindow.left
            bottom:pinorbiterWindow.bottom
            right:pinorbiterWindow.right
        }
        color:"black"
    }
    StyledText{
        id:user_ident
        text:"Enter PIN to Authorize user : "+requestedUser
        color:"White"
        fontSize: 24
        anchors{
            bottom:buttonContainer.top
            horizontalCenter: parent.horizontalCenter
        }
    }
    Item{
        id:buttonContainer
        height: scaleY(65)+50
        width: scaleX(50)
        //        color:"black"
        //        border.color:"white"
        //        border.width:1

        anchors{
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        Item{
            id:inputDisplay
            height: scaleY(10)
            anchors{
                left:parent.left
                right:parent.right
                top:parent.top
            }

            Rectangle{
                id:inputPhil
                anchors.fill: userPass
                color:userList.currentPrivatUser === -1 ? "yellow" : "green"
                opacity: .25
                radius:10
            }

            TextInput{
                id:userPass
                font.pointSize: inputDisplay.height*(.8)
                color: "black"
                echoMode: TextInput.Password
                fillColor: "transparent"

                anchors{
                    left:parent.left
                    right:parent.right
                    top:parent.top
                    bottom:parent.bottom
                    rightMargin: parent.width*.22
                    leftMargin: parent.width*.25
                }
            }
        }

        Item{
            id:buttonLayout
            anchors{
                top:inputDisplay.bottom
                left:parent.left
                right:parent.right
                bottom:parent.bottom
                topMargin: scaleY(1)

            }
            //            Rectangle{
            //                anchors.fill: parent
            //                color:"blue"
            //            }

            Column{
                id:verticalButtonLayout
                spacing: 10
                anchors{
                    top:parent.top
                    left:buttonLayout.left
                    right:buttonLayout.right
                    bottom:parent.bottom
                    bottomMargin: 40
                    rightMargin: buttonLayout.width*.25
                    leftMargin: buttonLayout.width*.25
                }

                Row{
                    id:topRow
                    height: parent.height /4
                    spacing:10
                    anchors{
                        left:parent.left
                        right:parent.right
                    }

                    Repeater{
                        model: [1, 2, 3]
                        delegate:PinButton{}
                    }
                }
                Row{
                    id:secondRow
                    height: parent.height /4
                    spacing:10
                    anchors{
                        left:parent.left
                        right:parent.right
                    }

                    Repeater{
                        model: [4, 5, 6]
                        delegate:PinButton{}
                    }
                }

                Row{
                    id:thirdRow
                    height: parent.height /4
                    spacing:10
                    anchors{
                        left:parent.left
                        right:parent.right
                    }
                    Repeater{
                        model: [7, 8, 9]
                        delegate:PinButton{}
                    }
                }

                Row{
                    id:fourthRow
                    height: parent.height /4
                    spacing: 10
                    anchors{
                        left:parent.left
                        right:parent.right
                    }
                    Repeater{
                        model: ["X", 0, "GO"]
                        delegate: PinButton{}
                    }
                }
            }
        }

    }

    Rectangle{
        height:75
        width: height
        color:"white"
        StyledText{
            text:"X"
            anchors.centerIn: parent
            color:"black"
        }
        anchors.left: parent.left
        anchors.top: buttonContainer.top
        MouseArea{
            anchors.fill: parent
            onClicked: pinorbiterWindow.state = "closed"
        }
    }

    states: [
        State {
            name: "loaded"
            PropertyChanges {
                target: phil
                opacity:0
            }
            PropertyChanges {
                target: buttonContainer
                opacity:0
            }

        },
        State {
            name: "ready"
            PropertyChanges {
                target: phil
                opacity:.85
            }
            PropertyChanges {
                target: buttonContainer
                opacity:1
            }

        },
        State {
            name: "closed"
            PropertyChanges {
                target: phil
                opacity:0
            }
            PropertyChanges {
                target: buttonContainer
                opacity:0
            }

        }
    ]

    transitions: [
        Transition {
            from: "loaded"
            to: "ready"
            PropertyAnimation{
                properties:"opacity"; duration: 350
            }
        },
        Transition {
            from: "ready"
            to: "closed"
            SequentialAnimation{
                PropertyAnimation{properties: "opacity"; duration:350}
                ScriptAction{script:{loadComponent("")}}
            }
        }
    ]




}
