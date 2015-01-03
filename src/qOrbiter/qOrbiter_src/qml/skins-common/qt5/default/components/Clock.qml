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
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()
    }



    StyledText{
        id: txtDate
        text: getDate()
        color: clockColor
        font.letterSpacing: 2
        font.family: appFont.name
        smooth: true
        width: parent.width
        anchors.centerIn: parent
        font.pixelSize: fontSize
        horizontalAlignment: Text.AlignHCenter
        font.bold: boldClock
    }
}

