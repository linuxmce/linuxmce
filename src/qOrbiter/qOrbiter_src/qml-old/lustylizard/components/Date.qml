import QtQuick 1.0
Rectangle {
    id: item2
property bool verticalMirror: false
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd, d.MM.yyyy");
    }
    Timer {
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }

    Text{
        id: txtDate
        text: getDate()
        anchors.right: parent.right
        color: "aliceblue"
        font.letterSpacing: 2
        smooth: true
        font.pointSize: 14
        height: headerbg.height
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
}
