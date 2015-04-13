import QtQuick 2.2
Item {
property bool verticalMirror: false
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd ,MMMM d| hh:mm ");
    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }

    Text{
        id: txtDate
        text: getDate()
        color: "aliceblue"
        font.letterSpacing: 2
        smooth: true
        anchors.centerIn: parent

    }
}
