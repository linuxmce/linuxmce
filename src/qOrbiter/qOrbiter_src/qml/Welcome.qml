import QtQuick 1.1

Item {
   height: appH
   width: appW
   Image {
       id: bg
       source: "qrc:/Splash.png"
   }
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
