import QtQuick 1.1
Item {
height: txtDate.paintedHeight
width: txtDate.paintedWidth
    function getDate(){
        var d = new Date();
        return Qt.formatDateTime(d, "dddd ,MMMM d - hh:mm ");
    }
    Timer { // Update the clock element periodically
        interval: 5; running: true; repeat: true
        onTriggered: txtDate.text = getDate()+ " - QOrbiter " +manager.iPK_Device
    }

    StyledText{
        id: txtDate
        text: getDate()
        smooth: true
        color:"white"
        fontSize: manager.isProfile ? scaleY(3) : scaleY(5)
    }
}
