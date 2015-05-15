import QtQuick 2.2
import "../components"
import "../."
import org.linuxmce.grids 1.0

StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: mediaFloorplan

    TabletFloorplanLayout{
        id: mediaFloorplan
        Component.onCompleted: {
            floorplan_devices.setCurrentFloorPlanType(5)
            floorplan_devices.clearAllSelections()
        }

        Row {
            id: rowButtons
            anchors.top: manager.isProfile ? parent.verticalCenter : parent.top
            anchors.topMargin: manager.isProfile ? undefined : 10
            anchors.left: manager.isProfile ? parent.left : parent.horizontalCenter
            anchors.leftMargin: 10
            anchors.right: parent.right
            spacing: 10
            StyledButton {
                id: btOff
                height: Style.appButtonHeight
                width: 100
                buttonText: "Stop"
                // call stopMedia with selected EA
                onActivated: {
                    var list = floorplan_devices.getSelectedDevice()
                    if (list.count > 0) {
                        console.log("manager.stopPlayback with EA = " + list.get(0).device)
                        manager.stopMediaOtherLocation(list.get(0).device)
                        activeStreams.model.refreshData()
                    }
                }
            }
/*            StyledButton {
                id: btRemote
                height: style.buttonHeight
                width: 100
                buttonText: "Remote"
                // open remote for this EA
                onActivated: {
                }
            }*/
        }

        GenericListModel {
            id:activeStreams
            label:qsTr("Media Streams")
            dataGrid: DataGrids.Floorplan_Media_Streams
            dataGridLabel:"mediaStreams"
            anchors{
                top: rowButtons.bottom
                left: rowButtons.left
                right:parent.right
                bottom:parent.bottom
                topMargin: 10
                rightMargin: 10
            }

            delegate:
                StyledButton{
                id:gridBtn
                buttonText: description
                textSize: 16
                height: Style.appButtonHeight
                width: parent.width

                onActivated: {
                    var list = floorplan_devices.getSelectedDevice()
                    if (list.length > 0) {
                        var eas = ''
                        for (var i = 0; i < list.length; i++)
                        {
                            if (i > 0)
                                eas += ','
                            eas += list.get(0).device
                        }
                        manager.doMoveMedia(eas,streamID)
                        activeStreams.model.refreshData()
                    }
                }
            }
        }
    }
}
