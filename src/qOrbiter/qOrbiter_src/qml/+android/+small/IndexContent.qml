import QtQuick 2.2
/*
  Root Item.
  */
Item{
    id:indexContent
    height:manager.appHeight
    width: manager.appWidth
    Component.onCompleted: {
        console.log("SmallUI loaded")
    }
    
    Image{
        fillMode: Image.PreserveAspectCrop
        source: "images/splash-bg.png"
        anchors.fill:parent
    }
    Rectangle{
        anchors.fill: parent
        opacity:.65
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
        text: qsTr("QOrbiter For Android\n\tSmall UI")
        color:"black"
        font.weight:Font.Light
        anchors{
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
    }
    
    Text{
        id:loadingText
        text:"Getting Started."
        color:"black"
        font.weight:Font.Light
        anchors{
            horizontalCenter: splashText.horizontalCenter
            top:splashText.bottom
        }

        font.pointSize: 32
    }
    
}
