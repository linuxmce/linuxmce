import QtQuick 2.1
Item {
width: txtDate.width
    property bool verticalMirror: false
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd ,MMMM d -- hh:mm:ss ");
    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }

    StyledText{
        id: txtDate
        text: getDate()
        color: "aliceblue"
        font.letterSpacing: 2
        smooth: true
        anchors.centerIn: parent
    }
}
