import QtQuick 1.0
import Qt.labs.shaders 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../effects"

Row{
    height: childrenRect.height
    width: childrenRect.width
    spacing: scaleX(1)
    
    AvOptionButton{
        buttontext: qsTr("Bookmarks")
    }
    AvOptionButton{
        buttontext: qsTr("Manage Playlist")
        MouseArea{
            anchors.fill: parent
            onClicked: playlist.optionVisible ? playlist.optionVisible =false: playlist.optionVisible=true
        }
    }
    AvOptionButton{
        buttontext: qsTr("Resend AV Codes")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/Avcodes.qml", storedAudioRemote)
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Attributes")
        MouseArea{
            anchors.fill: parent
            onClicked: textrect.visible = !textrect.visible
        }
    }
    AvOptionButton{
        buttontext: qsTr("Power")
    }
    HomeButton{}
}
