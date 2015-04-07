import QtQuick 1.0

Rectangle {
    width: 800
    height: 480
    Timer{
        id: destruct
        interval: 5000
        onTriggered: timertext = destruct.running.valueOf
    }

    Rectangle{
        anchors.fill: parent
        opacity: .5
        color: "yellow"
    }

    Text {
        id: errormsg
        text: "Orbiter Failed to Regen, please do it manually by visiting Http://dcerouter/lmce-admin/qOrbiterGenerator.php."

        anchors.centerIn: parent
        z:5
        font.pixelSize: 18
        }
    Text {
        id: timertext
        text: ": :"
        anchors.top: errormsg.bottom
    }

    }



