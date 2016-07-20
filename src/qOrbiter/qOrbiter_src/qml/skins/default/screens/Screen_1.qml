import QtQuick 2.2
import "../components"
import "../"
import org.linuxmce.screens 1.0
StyledScreen {
    id:screen
    onScreenOpen: manager.setBoundStatus(false)
    function showFloorplan(idx){

        switch(idx){
        case 0:manager.currentScreen=Number(Screens.Lights);   break;
        case 1:manager.currentScreen =Screens.Media;    break;
        case 2:manager.currentScreen=Screens.Climate;  break
        case 3:manager.currentScreen=Screens.Telephony;  break
        case 4:manager.currentScreen=Screens.Security; break;
        default: break;
        }
    }
    NowPlayingButton {
        id: now_playing_button
    }

    GenericListModel {
        id: genericListContainer
        label: qsTr("Scenarios for  %1 ").arg(roomList.currentEA)
        anchors{
            left:parent.left
            top:now_playing_button.bottom
            right:parent.right
            bottom:parent.bottom
        }

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
                font.pointSize: Style.appFontSize_title//manager.isProfile ? Style.scaleY(3) : Style.scaleY(5)
                text:name

                MouseArea{
                    anchors.fill: parent
                    onClicked:showFloorplan(index)
                }
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
