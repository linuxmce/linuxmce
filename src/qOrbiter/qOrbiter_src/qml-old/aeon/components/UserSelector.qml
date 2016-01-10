import QtQuick 1.1

Rectangle {
    id: recScreen
    height: skinStyle.orbiterH
    width: skinStyle.orbiterW
    color: "transparent"
    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: .5
        MouseArea {
            anchors.fill: parent
        }
    }

    Rectangle {
        id: recUsers
        x: mnuBottomRight.x+txtUser.x
        y: mnuBottomRight.y+txtUser.y
        height: txtUser.height
        width: txtUser.width
        opacity: 0
        color: "transparent"
        Image {
            source: '../img/dialogs/progress_backg.png'
            anchors.fill: parent
            Flickable {
                anchors.centerIn: parent
                height: parent.height*.764
                width: parent.width*.972
                flickableDirection: "HorizontalFlick"
                Rectangle {
                    id: recTitle
                    width: parent.width
                    height: parent.height/5
                    color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        color: "white"
                        text: " Select a user"
                        font.family: scout.name;
                        font.pixelSize: parent.height*.7;
                        smooth: true
                    }
                }

                ListView{
                    id:roomsource
                    anchors.fill: parent
                    height: parent.height-recTitle.height
                    anchors.topMargin: recTitle.height
                    orientation: ListView.Horizontal
                    spacing: 5
                    clip: true
                    model: userList
                    delegate: Component {
                        Rectangle{
                            height:roomsource.height
                            width: roomsource.width/4
                            color: skinStyle.not_color
                            Image {
                                source: "../../../../img/icons/personal.png"
                                anchors.fill: parent
                                fillMode: Image.PreserveAspectFit
                            }
                            Rectangle{
                                height: 20
                                width: parent.width
                                color: "transparent"
                                anchors.top: parent.top
                                Text {
                                    anchors.centerIn: parent
                                    width: parent.width
                                    text: username
                                    wrapMode: "WrapAnywhere"
                                    font.family: scout.name;
                                    font.pixelSize: parent.height*.7;
                                    smooth: true
                                }
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    setCurrentUser(index+1)
                                    //currentuser = title
                                    //txtUser.text = title
                                    recUsers.state = 'DEFAULT'
                                    //stage.visible = true;
                                    timerDestroy.start()
                                }
                            }

                        }
                    }
                }
            }
        }

        states: [
            State {
                name: "LOADED"
                PropertyChanges { target: recUsers; height: scaleY(38.19) }
                PropertyChanges { target: recUsers; width: scaleX(82.97) }
                PropertyChanges { target: recUsers; x: recScreen.width/2-recUsers.width/2}
                PropertyChanges { target: recUsers; y: recScreen.height/2-recUsers.height/2}
                PropertyChanges { target: recUsers; opacity: 1}
            }
        ]
        transitions: [ Transition {
                to: 'LOADED'
                NumberAnimation { duration: 300; target: recUsers; properties: "x,y"; easing.type: Easing.InOutQuad }
                NumberAnimation { duration: 300; target: recUsers; easing.amplitude: 0.5; properties: "height,width"; easing.type: Easing.InOutQuart }
                NumberAnimation { duration: 300; target: recUsers; properties: "opacity" }
                reversible: true
            }
        ]

    }
    Component.onCompleted: {
        recUsers.state = 'LOADED'
        //stage.visible = false;
    }
    Timer {
        // delay component destruction until state transition is complete
        id: timerDestroy
        interval: 300; running: false; repeat: false
        onTriggered: {
            recScreen.destroy()
        }
    }
}
