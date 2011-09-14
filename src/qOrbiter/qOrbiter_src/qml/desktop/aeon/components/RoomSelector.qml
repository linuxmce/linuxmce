import QtQuick 1.0

Rectangle {
    id: recScreen
    height: style.orbiterH
    width: style.orbiterW
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
        id: recRooms
        x: mnuBottomRight.x+txtRoom.x
        y: mnuBottomRight.y+txtRoom.y
        height: txtRoom.height
        width: txtRoom.width
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
                        text: " Select a room"
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
                    model: roomList
                    delegate: Component {
                        Rectangle{
                            height:roomsource.height
                            width: roomsource.width/4
                            color: style.not_color
                            Image {
                                source: room_image
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
                                    text: title + ": In EA: " + entertain_area
                                    wrapMode: "WrapAnywhere"
                                    font.family: scout.name;
                                    font.pixelSize: parent.height*.7;
                                    smooth: true
                                }
                            }
                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    setActiveRoom(intRoom, entertain_area)
                                    currentroom = title
                                    txtRoom.text = title
                                    recRooms.state = 'DEFAULT'
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
                PropertyChanges { target: recRooms; height: scaleY(38.19) }
                PropertyChanges { target: recRooms; width: scaleX(82.97) }
                PropertyChanges { target: recRooms; x: recScreen.width/2-recRooms.width/2}
                PropertyChanges { target: recRooms; y: recScreen.height/2-recRooms.height/2}
                PropertyChanges { target: recRooms; opacity: 1}
            }
        ]
        transitions: [ Transition {
                to: 'LOADED'
                NumberAnimation { duration: 300; target: recRooms; properties: "x,y"; easing.type: Easing.InOutQuad }
                NumberAnimation { duration: 300; target: recRooms; easing.amplitude: 0.5; properties: "height,width"; easing.type: Easing.InOutQuart }
                NumberAnimation { duration: 300; target: recRooms; properties: "opacity" }
                reversible: true
            }
        ]

    }
    Component.onCompleted: {
        recRooms.state = 'LOADED'
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
