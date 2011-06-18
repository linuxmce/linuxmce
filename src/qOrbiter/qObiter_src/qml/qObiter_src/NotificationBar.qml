import QtQuick 1.0

Rectangle {
    color: "lightblue"
    width: parent - 5
    height: 25
    anchors.top: parent.top

    Connections {
        target: clock

    }

    Text {
        id: mnuTitle
        text: "qOrbiter"
        color: "black"
        font.pointSize: 16
    }

    Text {
        id: weather
        text: "Weather"
        anchors.centerIn: parent
    }

    Text {

        id: clock
        text: currentDateTime
        font.pointSize: 12
        anchors.right: parent.right
    }

}
