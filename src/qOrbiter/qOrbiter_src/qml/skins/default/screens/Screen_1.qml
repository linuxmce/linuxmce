import QtQuick 2.3
import "../components"
import "../"
import org.linuxmce.screens 1.0
StyledScreen {
    id:screen



    StyledButton{
        buttonText: qsTr("Advanced")
        state:"fixed"
        anchors.centerIn: parent
        onActivated: manager.setCurrentScreen(Screens.AdvancedOptions)
    }

    GenericListModel {
        id: genericListContainer
        label: qsTr("Scenarios")
        width: parent.width
        anchors{
            left:parent.left
            verticalCenter: parent.verticalCenter
        }
        height: Style.scaleY(75)

        model:qmlRoot.scenarios
        delegate:  Item {
            height: Style.scaleY(10)
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
                text:name
            }
            ListView{
                height: parent.height
                anchors{
                    left:parent.left
                    leftMargin: Style.scaleX(15)
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
                delegate:Item{
                    height: parent.height
                    width: Style.scaleX(12)

                    StyledButton{
                        buttonText:title
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
