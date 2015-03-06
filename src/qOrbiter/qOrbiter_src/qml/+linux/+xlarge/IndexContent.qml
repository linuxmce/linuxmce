import QtQuick 2.3

/*
  Root Item.
  */
Item{
    id:indexContent
    anchors.fill: parent
    Component.onCompleted: console.log("Linux window open")
    Image{
        source: "images/splash-bg.png"
        anchors.fill:parent
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

    Text {
        id: splashText
        text: qsTr("QOrbiter For Linux Desktop")
        font.pointSize: 55
        color:"black"
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
