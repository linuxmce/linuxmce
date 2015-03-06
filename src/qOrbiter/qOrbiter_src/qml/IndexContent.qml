import QtQuick 2.3


/*
  Root Item.
  */
Item{
    id:indexContent
    anchors.fill: parent
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
    
    Image{
        source: "images/splash-bg.png"
        anchors.fill:parent
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
        text:"Please Wait, Loading"
        color:"white"
        anchors.centerIn: parent
        font.pointSize: 32
    }
    
}
