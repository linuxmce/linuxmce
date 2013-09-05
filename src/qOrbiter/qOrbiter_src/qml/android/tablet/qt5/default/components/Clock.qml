import QtQuick 2.0

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
    height: childrenRect.height
    width: childrenRect.width
    StyledText{
        id: txtDate
        text: getDate()
        color: "aliceblue"
        font.letterSpacing: 1.5
        smooth: true
        anchors.centerIn: parent
        fontSize: scaleY(5)

    }
}

