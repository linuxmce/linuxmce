import QtQuick 1.1
import "../../../skins-common/lib/handlers"
import "../js/ComponentLoader.js" as MyJs
Rectangle {
    width: optionsDisplay.width
    height: optionsDisplay.height
    color:"transparent"
    Flickable{
        height: parent.height
        width: parent.width
        flickableDirection: Flickable.HorizontalFlick


        Row{
            height: childrenRect.height
            width: childrenRect.width
            spacing: scaleX(1)

            AvOptionButton{
                buttontext: qsTr("Zoom & Aspect")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        MyJs.createStageComponent("../components/ZoomAspect.qml", mediaPlaybackBase)
                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Bookmarks")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {

                        manager.ShowBookMarks()
                        MyJs.createStageComponent("../components/Bookmarks.qml", mediaPlaybackBase)

                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Resend AV Codes")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        MyJs.createStageComponent("../components/Avcodes.qml", mediaPlaybackBase)
                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Manage Playlist")
                MouseArea{
                    anchors.fill: parent
                    onClicked: mediaPlaybackBase.togglePlaylistEditor()
                }
            }
            AvOptionButton{
                buttontext: qsTr("Thumbnail")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        manager.grabFileImage()
                        MyJs.createStageComponent("../components/AssignScreenShot.qml", mediaPlaybackBase)

                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Jog")
                MouseArea{
                    anchors.fill: parent
                    onClicked:  {
                        MyJs.createStageComponent("../components/JogPanel.qml", mediaPlaybackBase)
                    }
                }
            }
            AvOptionButton{
                buttontext: qsTr("Power")
            }

            AvOptionButton{
                buttontext: "Previous Screen"
                ScreenBackHandler {
                }
            }
        }
    }
}
