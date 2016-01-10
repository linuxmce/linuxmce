import QtQuick 1.1
Item {
    height: childrenRect.height
    width: childrenRect.width
property bool verticalMirror: false
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd ,MMMM d| hh:mm ");
    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }

    StyledText{
        id: txtDate
        textLabel: getDate()
        color: "aliceblue"
        font.letterSpacing: 2
        smooth: true
        anchors.centerIn: parent

    }
}
