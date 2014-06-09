import QtQuick 1.0

Item {
    property bool verticalMirror: false
    property color clockColor:"white"
    property int clockFontSize: 32
    property bool boldClock:false
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd ,MMMM d || hh:mm ");
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
        color: clockColor
        font.letterSpacing: 2
        font.family: appFont.name
        smooth: true
        anchors.centerIn: parent
        font.pixelSize: clockFontSize
        font.bold: boldClock
    }
}

