import QtQuick 2.2
import "../components"
import QtQuick.Controls 1.2
import "../."
StyledScreen{
    id:screenContainer
    navigation: "FloorplanNav.qml"
    property alias floorplan: lightsFloorplan
    //  onScreenOpening: floorplan_devices.clearAllSelections()
    onScreenClosing: floorplan_devices.setCurrentFloorPlanType(-1)

    Item{
        anchors.fill: parent
        id:outerContainer

        Component.onCompleted: {
            floorplan_devices.setCurrentFloorPlanType(2)
            state="visual"

            if(floorplan_pages.length===0){
                outerContainer.state="list"
            } else {
                outerContainer.state="visual"
            }
            console.log(state)
        }


        TabletFloorplanLayout {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: /*manager.isProfile ? buttonRow.top : */parent.bottom
            anchors.right: /*manager.isProfile ?*/ parent.right //: buttonRow.right
            id: lightsFloorplan
            Component.onCompleted: {
                floorplan_devices.setCurrentFloorPlanType(2)
                // manager.getFloorplanDevices(2)
            }
        }

        Item{
            id:floorplan_listView
            anchors{
                top:parent.top
                left:parent.left
                right:parent.right
                bottom:parent.bottom
            }

            GenericListModel{
                model:floorplan_devices
                label: qsTr("Device List. %1 Devices").arg(modelCount)
                anchors.fill: parent
                sectionProperty:"room"
                delegate:
                    Row{
                    property variant dynData;
                    Component.onCompleted: {
                        dynData = floorplan_devices.getDeviceData(deviceno);

                    }
                    width:parent.width
                    height: Style.listViewItemHeight
                    Image {
                        id: fpDevice_image
                        source:type==="52" ? "" : "../images/floorplan/"+type+".png"
                        sourceSize:Qt.size(Style.scaleY(6), Style.scaleY(6))
                        anchors.verticalCenter: parent.verticalCenter
                        height: 50
                        width: height
                        cache: false
                        opacity: selected ? 1 : .24
                    }
                    StyledButton{
                        width: parent.width/2
                        height: parent.height
                        buttonText: qsTr("%1 is %2").arg( name).arg(dynData.deviceState)+deviceno
                        fontSize: 18
                    }
                    CheckBox{
                        id:chk
                        anchors.verticalCenter: parent.verticalCenter
                        checked:dynData.selected
                        onCheckedChanged: if(!checked) { cmds.clear() }
                        onClicked: { floorplan_devices.setDeviceSelection(deviceno) ;/* manager.getFloorplanDeviceCommands(deviceno)*/}

                    }
                    ListModel{
                        id:cmds
                    }

                    Connections{
                        target:floorplan_devices
                        onDeviceCommandsChanged:{
                            if(!chk.checked)return

                            cmds.clear()
                            var cl = floorplan_devices.getDeviceCommands(deviceno).commands
                            for(var c in cl){
                                cmds.append(cl[c])
                            }
                        }
                    }

                    ComboBox{
                        currentIndex: 2
                        model:cmds
                        anchors.verticalCenter: parent.verticalCenter
                        enabled: model.count!==0
                        textRole: "command_name"
                        onCurrentIndexChanged: {
                            if(count==0)return
                            console.log(cmds.get(currentIndex).command_number)
                        }
                    }
                }
            }

        }

        states: [
            State {
                name: "visual"
                PropertyChanges {
                    target: lightsFloorplan
                    visible:true
                }
                PropertyChanges {
                    target: screenContainer
                    navigation:"FloorplanNav.qml"
                }
                PropertyChanges{
                    target: floorplan_listView
                    visible:false
                }
            },
            State {
                name: "list"
                PropertyChanges {
                    target: lightsFloorplan
                    visible:false
                }PropertyChanges {
                    target: screenContainer
                    navigation:""
                }
                PropertyChanges {
                    target: floorplan_listView
                    visible:true
                }
            }
        ]
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


