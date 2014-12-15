import QtQuick 2.2
import QtQuick.Window 2.2
import DceScreenSaver 1.0
Window {
   flags:Qt.WindowFullScreen
   Rectangle{
   anchors.fill: parent
   color: "white"
   }

    Rectangle{
        anchors.fill: parent
        opacity:.35
        gradient:  Gradient{
            GradientStop{
                position: 0.0
                color:"transparent"
            }
            GradientStop{
                position:.75
                color:"black"
            }
        }
    }

    Text{
        id:welcomeText1
        anchors.centerIn: parent
        font.weight: Font.Light
        color:"white"
        font.pixelSize: 36
        text:"LinuxMCE on \n iOS"
    }



        Text{
            id:goText

            font.weight: Font.Light
            color:"white"
            font.pixelSize: 36
            text:"Please Wait"
            anchors{
                top:welcomeText1.bottom
                horizontalCenter: welcomeText1.horizontalCenter
                topMargin: 100
            }
        }


}
