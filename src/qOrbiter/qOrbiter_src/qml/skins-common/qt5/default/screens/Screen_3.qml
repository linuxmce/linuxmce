import QtQuick 2.2
import "../components"
import org.linuxmce.grids 1.0

StyledScreen {
    navigation: "FloorplanNav.qml"
    property alias floorplan: mediaFloorplan

    TabletFloorplanLayout{
        id: mediaFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            setNavigation();
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
                height: style.buttonHeight
                width: 100
                buttonText: "Stop"
                // call stopMedia with selected EA
                onActivated: {
                    var list = mediaFloorplan.selectedDevices
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

        ListView {
            id:activeStreams
            anchors{
                top: rowButtons.bottom
                left: rowButtons.left
                right:parent.right
                bottom:parent.bottom
                topMargin: 10
                rightMargin: 10
            }
            model: manager.getDataGridModel("mediaStreams", DataGrids.Floorplan_Media_Streams)
            delegate:
                StyledButton{
                id:gridBtn
                buttonText: description
                textSize: 16
                height: style.buttonHeight
                width: parent.width

                onActivated: {
                    var list = mediaFloorplan.selectedDevices
                    if (list.count > 0) {
                        var eas = ''
                        for (var i = 0; i < list.count; i++)
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
