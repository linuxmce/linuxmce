import QtQuick 2.2
import "../components"
import "../"
import org.linuxmce.screens 1.0
StyledScreen {
    id:screen

    NowPlayingButton {
        id: now_playing_button
    }

    GenericListModel {
        id: genericListContainer
        label: qsTr("Scenarios in  %1").arg(roomList.currentRoom)
        anchors{
            left:parent.left
            top:now_playing_button.bottom
            right:parent.right
        }
        height: Style.appButtonHeight *6

        model:qmlRoot.scenarios
        delegate:  Item {
            height: Style.appButtonHeight
            width: parent.width
            Rectangle{
                anchors.fill: parent
                color:Style.appcolor_background_list
                opacity: Style.appList_opacity
            }

            StyledText{
                id:rowLabel
                anchors{
                    left:parent.left
                    verticalCenter: parent.verticalCenter
                }
                font.pointSize: Style.appFontSize_list//manager.isProfile ? Style.scaleY(3) : Style.scaleY(5)
                text:name
            }
            ListView{
                height: parent.height
                anchors{
                    left:rowLabel.right
                    leftMargin: Style.scaleX(5)
                    right:parent.right
                    verticalCenter: parent.verticalCenter
                }
                model: switch(floorplantype) {
                       case 2:currentRoomLights; break;
                       case 5:currentRoomMedia; break;
                       case 1:currentRoomClimate; break
                       case 3:currentRoomTelecom; break
                       case 4:currentRoomSecurity; break;
                       default: undefined;
                       }

                orientation:ListView.Horizontal
                clip:true
                delegate:Item{
                    height: parent.height
                    // width: Style.appButtonWidth
                    width: Style.appButtonWidth
                    StyledButton{
                        buttonText:title
                        width:parent.width
                        state:"scenario"
                        anchors.fill: parent
                        anchors.margins: 5
                        onActivated: manager.execGrp(params)
                    }
                }
            }

        }
    }

}
