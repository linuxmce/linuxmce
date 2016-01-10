import QtQuick 1.0
Item {
    id: item1
    property bool verticalMirror: false
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "hh:mm");
    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }

    Text{
        id: txtDate
        text: getDate()
        color: "#ffffff"
        font.letterSpacing: 2
        font.pointSize: 14
        smooth: true
        width: 1
        height: headerbg.height
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
}
