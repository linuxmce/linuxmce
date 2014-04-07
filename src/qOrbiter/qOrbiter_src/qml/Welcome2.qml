import QtQuick 2.0

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
               color:"black"
           }
           GradientStop{
               position: .56
               color:"transparent"
           }
           GradientStop{
               position: .75
               color:"black"
           }
       }
   }

    Text{
        text:"Please Wait, Loading"
        color:"white"
        anchors.centerIn: parent
        font.pointSize: 32
    }
}
