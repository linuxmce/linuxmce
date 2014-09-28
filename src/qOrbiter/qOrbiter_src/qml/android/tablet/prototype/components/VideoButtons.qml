import QtQuick 1.0
import "../components"
import "../js/ComponentLoader.js" as MyJs
import com.nokia.android 1.1


Row{
    height: childrenRect.height
    width: childrenRect.width
    spacing: scaleX(1)
    AvOptionButton{
        buttontext: qsTr("Zoom & Aspect")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/ZoomAspect.qml", storedVideoRemote)
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Bookmarks")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                
                manager.ShowBookMarks()
                MyJs.createStageComponent("../components/Bookmarks.qml", storedVideoRemote)
                
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Resend AV Codes")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/Avcodes.qml", storedVideoRemote)
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Manage Playlist")
        MouseArea{
            anchors.fill: parent
            onClicked: playlist.optionVisible ? playlist.optionVisible =false: playlist.optionVisible=true
        }
    }
    AvOptionButton{
        buttontext: qsTr("Thumbnail")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                manager.grabScreenshot(dcenowplaying.filepath)
                MyJs.createStageComponent("../components/AssignScreenShot.qml", storedVideoRemote)
                
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Jog")
        MouseArea{
            anchors.fill: parent
            onClicked:  {
                MyJs.createStageComponent("../components/JogPanel.qml", storedVideoRemote)
            }
        }
    }
    AvOptionButton{
        buttontext: qsTr("Power")
    }
    HomeButton{}
    
}
