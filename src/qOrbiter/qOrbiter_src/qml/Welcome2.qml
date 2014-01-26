import QtQuick 2.0

Item {
   height: 1024
   width: 768

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
