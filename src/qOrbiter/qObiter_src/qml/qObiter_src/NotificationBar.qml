import QtQuick 1.0

Rectangle {
    property alias clocktime : clock.text
    color: "lightblue"
    width: 640
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

    Text {
        id: text1
        x: 452
        y: 5
        text:"dceObject.iPk_User"
        font.pixelSize: 12
    }

}
