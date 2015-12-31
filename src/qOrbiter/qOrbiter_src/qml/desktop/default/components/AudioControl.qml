import QtQuick 1.1

Rectangle {
    id:audioControl
    width: 50
    height: 50
    color:"transparent"
    state:"inactive"

    Timer{
        id:inactivity_timer
        interval:5000
        onTriggered: audioControl.state="inactive"
        running:false
    }

    Image {
        id: muteimg
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        height: scaleY(9)
        width: scaleX(5)
        source: "../img/icons/kmix.png"

        StyledText{
            text: manager.discreteAudio ? manager.deviceVolume : ""
            color:"green"
            fontSize: 42
            font.bold: true
            anchors.centerIn: parent
        }
        MouseArea{
            anchors.fill: parent
            hoverEnabled: true
            onPressed: audioControl.state="active"
            onEntered: audioControl.state="active"
        }
    }

    Rectangle{
        id:dragbar
        height: 10
        width: 0
        color: "green"
        opacity: 0
        anchors.left: muteimg.right
        anchors.verticalCenter: muteimg.verticalCenter

        Loader{
            id:handleLoader
            anchors.verticalCenter: parent.verticalCenter
            source: manager.discreteAudio ? "DiscreteDragHandle.qml" : "BinaryDragHandle.qml"
        }
    }

    states: [
        State {
            name: "inactive"
            PropertyChanges {
                target: audioControl
                width:50
            }
        },
        State {
            name: "active"
            PropertyChanges {
                target: audioControl
                width:200
            }
            PropertyChanges {
                target: inactivity_timer
                running:true
            }
            PropertyChanges {
                target: dragbar
                opacity:1
                width:150
            }
        }
    ]
}
