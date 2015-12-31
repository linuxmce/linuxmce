import QtQuick 1.1

Item {
   height: appH
   width: appW


   Image {
       id: bg
       source: "qrc:../img/Splash.png"
   }
   Rectangle{
       anchors.fill: parent
       opacity:.85
       gradient: Gradient{
           GradientStop{
               position: 0.0
               color:"transparent"
           }
           GradientStop{
               position: .56
               color:"black"
           }
       }
   }


    Text{
        text:"Please Wait, Loading LinuxMCE"
        color:"white"
        anchors.centerIn: parent
        font.pointSize: 32
    }


}
