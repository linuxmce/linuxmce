import QtQuick 2.2
import QtQuick.orbiterWindow 2.2

Item {
   anchors.fill: parent

   Rectangle{
   anchors.fill: parent
   color: "lightgrey"
   }

    Rectangle{
        anchors.fill: parent
        opacity:.55
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
