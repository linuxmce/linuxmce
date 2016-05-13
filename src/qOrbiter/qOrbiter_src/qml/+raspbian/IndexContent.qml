import QtQuick 2.2
/*
  Root Item.
  */
Item{
    id:indexContent
    anchors.fill: parent
    Image{
        source: "images/splash-bg.png"
        anchors.fill:parent
    }
    Rectangle{
        anchors.fill: parent

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


    Text {
        id: splashText
        text: qsTr("QOrbiter")
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
    }

    Text{
        id:loadingText
        text:"Please Wait, Loading RaspberryPi as MD!!"
        color:"red"
        font.bold: true
        anchors.centerIn: parent
        font.pointSize: 32
    }

}
