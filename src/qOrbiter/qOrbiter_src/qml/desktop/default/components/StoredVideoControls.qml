import QtQuick 1.1



import "../components"
import "../js/ComponentLoader.js" as MyJs
import "../../../skins-common/lib/handlers"


Row{
    id:controlrow
    anchors.bottom: parent.bottom
    anchors.bottomMargin: scaleY(1)
    height: childrenRect.height
    width: childrenRect.width
    anchors.horizontalCenter: parent.horizontalCenter
    z:0
    Column{
        height: childrenRect.height
        width: childrenRect.width
        spacing: scaleY(1.5)
        
        VideoControls {
            id: videocontrols1
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Row{
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleX(1)
            AvOptionButton{
                buttontext: qsTr("Zoom & Aspect")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        MyJs.createStageComponent("../components/ZoomAspect.qml", storedvideoremote)
                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Bookmarks")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        
                        manager.ShowBookMarks()
                        MyJs.createStageComponent("../components/Bookmarks.qml", storedvideoremote)
                        
                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Resend AV Codes")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        MyJs.createStageComponent("../components/Avcodes.qml", storedvideoremote)
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
                        manager.grabStreamImage(dcenowplaying.path)
                        MyJs.createStageComponent("../components/AssignScreenShot.qml", storedvideoremote)
                        
                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Jog")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        MyJs.createStageComponent("../components/JogPanel.qml", storedvideoremote)
                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Power")
            }
            HomeButton{}
            
            AvOptionButton{
                buttontext: "Previous Screen"
                ScreenBackHandler {
                }
            }
        }
    }
    
}
