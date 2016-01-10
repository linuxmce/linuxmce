import QtQuick 1.1

Rectangle {
    property alias clocktime : clock.text

    color: "skyblue"
    width: 800
    height: 45
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
        Image {
            id: weatherimg
            x: 5
            y: -15
            source: "../img/icons/kweather.png"
            height:45
            anchors.topMargin: -15
            width: 45
            anchors.top: parent.top
        }
        anchors.centerIn: parent
    }

    Text {

        id: clock
        text: currentDateTime
        font.pointSize: 12
        anchors.right: parent.right
    }




}
