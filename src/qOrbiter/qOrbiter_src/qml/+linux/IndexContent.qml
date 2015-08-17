import QtQuick 2.2
import QtGraphicalEffects 1.0

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
        text: qsTr("QOrbiter For Linux")
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
        font.pointSize: 22
    }

    Column{
        id:statusCol

        anchors{
            right:parent.right
            verticalCenter: parent.verticalCenter
        }

        height: parent.height*.75
        width: parent.width *.35


        Text {
            id: deviceSettings
            text: qsTr("Device Settings")
            color:settings.ready ? "green" : "red"          
        }

    }
    PropertyAnimation{
        target: spinner
        running: true
       property: "rotation"
        from:0
        to:360
        loops: Animation.Infinite
        duration: 750
    }
    Rectangle{
        id:spinner
        color:"white"
        height: 100
        width: 10
        anchors{
            left:parent.left
            verticalCenter: parent.verticalCenter
        }
    }
    
}
