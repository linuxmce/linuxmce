import QtQuick 2.0

Item {
   height: appH
   width: appW

    Rectangle{
        anchors.fill: parent
        color:"black"
    }

    Text{
        text:"Please Wait, Loading"
        color:"white"
        anchors.centerIn: parent
        font.pointSize: 32
    }
}
