import QtQuick 2.2
import QtQuick.Controls 1.3

import "../components"
StyledScreen{
    navigation: "FloorplanNav.qml"
    property alias floorplan: lightsFloorplan

    TabletFloorplanLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: /*manager.isProfile ? buttonRow.top : */parent.bottom
        anchors.right: /*manager.isProfile ?*/ parent.right //: buttonRow.right
        id: lightsFloorplan
        Component.onCompleted: {
            hideInfoPanel();
            floorplan_devices.clearAllSelections()
        }

    }
/*    Item {
        id: buttonRow
        anchors.right: parent.right
        anchors.left: undefined
        anchors.top: manager.isProfile ? undefined : parent.top
        anchors.bottom: parent.bottom
        height: manager.isProfile ? style.buttonHeight : undefined
        width: manager.isProfile ? parent.width : style.buttonWidth

        Grid {
            columns: manager.isProfile ? 4 : 1
            rows: manager.isProfile ? 1 : 4
            spacing: 3
            StyledButton {
                label: "On"
                onActivated: {
                    var list = lightsFloorplan.selectedDevices
                    for (var i = 0; i < list.count; i++) {
                        manager.turnOn(list.get(i).device);
                    }
                }
            }
            StyledButton {
                label: "Off"
                onActivated: {
                    var list = lightsFloorplan.selectedDevices
                    for (var i = 0; i < list.count; i++) {
                        manager.turnOff(list.get(i).device);
                    }
                }
            }
            StyledButton {
                label: "Level"
                onActivated: {
                    levelSlider.visible = !levelSlider.visible
                }
            }
        }
        Rectangle {
            id: levelSlider
            color: style.darkColor
            anchors.right: manager.isProfile ? parent.right : buttonRow.left
            anchors.top: manager.isProfile ? undefined : buttonRow.top
            anchors.bottom: manager.isProfile ? buttonRow.top :  buttonRow.bottom
            anchors.margins: 6
            height : style.buttonHeight*6
            width: 40
            visible: false

            Slider {
                anchors.fill: parent
                updateValueWhileDragging: false
                value: 0.5
                orientation: Qt.Vertical
                onValueChanged: {
                    var list = lightsFloorplan.selectedDevices
                    for (var i = 0; i < list.count; i++) {
                        manager.setLevel(list.get(i).device, value*100);
                    }

                }
            }
        }
    }
*/
}


