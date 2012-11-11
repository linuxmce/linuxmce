import QtQuick 2.0
import "../components"
import "../js/ComponentLoader.js" as MyJs

Row{
    id:controlrow
    anchors.bottom: parent.bottom
    anchors.bottomMargin: scaleY(5)
    height: controlcol.height
    width: childrenRect.width
    anchors.horizontalCenter: parent.horizontalCenter
    Column{
        id:controlcol
        height: childrenRect.height
        width: childrenRect.width
        spacing: scaleY(1)
        // VideoControls {
        //                id: videocontrols1
        //            }
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
                        MyJs.createStageComponent("../components/Avcodes.qml", storedaudioremote)
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
    }
}
