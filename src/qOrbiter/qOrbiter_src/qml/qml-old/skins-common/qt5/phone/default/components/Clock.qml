import QtQuick 2.2

Item {
    property bool verticalMirror: false
    property color clockColor:"white"   
    property bool boldClock:false
    width:manager.appWidth /3
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd ,MMMM d \n hh:mm:ss -- yyyy ");
    }

    Timer { // Update the clock element periodically
        interval: 1000; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }



    StyledText{
        id: txtDate
        color: clockColor
        font.letterSpacing: 2
        font.family: appFont.name
        smooth: true
        width: parent.width
        anchors.centerIn: parent
        font.pointSize: skinstyle.clockFontSize
        horizontalAlignment: Text.AlignHCenter
        font.bold: boldClock
    }
}

