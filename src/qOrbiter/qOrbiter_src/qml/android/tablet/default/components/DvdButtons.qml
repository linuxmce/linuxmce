import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
Row{
    height: childrenRect.height
    width: childrenRect.width
    spacing: scaleX(1)
    AvOptionButton{
        buttontext: qsTr("Zoom & Aspect")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/ZoomAspect.qml", dvd_remote)
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Bookmarks")
    }
    AvOptionButton{
        buttontext: qsTr("Resend AV Codes")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/Avcodes.qml", dvd_remote)
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Menu")
        MouseArea{
            anchors.fill: parent
            onClicked: manager.dvd_showMenu(true)
        }
    }
    AvOptionButton{
        buttontext: qsTr("Jog")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/JogPanel.qml", dvd_remote)
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Dvd Options")
    }
    AvOptionButton{
        buttontext: qsTr("Copy Disc")
    }
    AvOptionButton{
        buttontext: qsTr("Bookmarks")
    }
    AvOptionButton{
        buttontext: qsTr("View Attributes")
    }
    AvOptionButton{
        buttontext: qsTr("Thumbnail")
    }
    
    AvOptionButton{
        buttontext: qsTr("Power")
    }
    HomeButton{}
}
