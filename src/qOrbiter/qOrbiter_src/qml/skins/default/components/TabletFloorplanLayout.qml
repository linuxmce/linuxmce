import QtQuick 2.2
import org.linuxmce.grids 1.0
import "../."
Panel{
    id:fp_panel
    headerTitle:qsTr("Floorplan")

    buttonControls: Row {
        StyledButton{
            buttonText: qsTr("Show List")
            height: Style.appNavigation_panelHeight
            onActivated:{
                outerContainer.state="list"
            }

        }
        GenericListModel{
            id:pageView
            model: floorplan_pages
            label: qsTr("Floorplan(s) : %1").arg(modelCount)
            width: Style.scaleX(25)
            height: Style.scaleY(45)
            delegate:Rectangle{
                height: Style.scaleY(9)
                width:parent.width
                color: Style.appcolor_background
                border.color: Style.appcolor_foregroundColor
                StyledText {
                    id: desc
                    text: m_description
                    width: parent.width
                    height: parent.height
                    fontSize: Style.fontSize_small
                }
                MouseArea{
                    anchors.fill: parent
                    onPressed:{
                        floorplan_devices.clearAllSelections()
                        floorplan_devices.setCurrentPage(m_page)
                    }
                }
            }
        }
    }

    content: Item{
        id:phoneFloorplanLayout
        anchors{
            top:parent.top
            bottom: parent.bottom
            left:parent.left
            right:parent.right
        }

        focus:true
        property bool rotated:manager.isProfile
        property string imageDir:""
        property alias selectedDevices:selections
        state:"floorplanView"
        Component.onCompleted: {
            forceActiveFocus();
            processSelectedDevices()

        }

        Connections{
            target:floorplan_devices
            onSelectedDevicesChanged:{processSelectedDevices(); floorplan_devices.updateDeviceData()}
        }
        Connections{
            target:floorplan_devices
            onFloorplanTypeChanged:{
                activeStreams.visible = floorplan_devices.floorplanType===5
                if (activeStreams.visible) {
                    activeStreams.dataGrid = DataGrids.Floorplan_Media_Streams
                }
                row_button_scroll.visible = activeStreams.visible
            }
        }

        Keys.onReleased: {
            event.accepted=true
            switch(event.key){
            case Qt.Key_Menu:
                phoneFloorplanLayout.state="commandView"

                break;
            default:
                console.log("Recieved key ==>"+event.key+" but not accepting")
                break;
            }
        }
        function processSelectedDevices(){
            selections.clear()

            for (var prop in floorplan_devices.selectedDevices){
                //            console.log(prop)
                //            console.log(floorplan_devices.selectedDevices[prop])
                selections.append({"device":prop, "name":floorplan_devices.selectedDevices[prop]})
            }
            sendCommandBox.refreshCommands()
        }


        ListModel{
            id:selections
        }

        ParamManager{
            id:requestParamManager
        }

        FloorPlandisplayTemplate{
            id:floorplanPic
            useList: true
        }

        GenericListModel {
            id:activeStreams
            label:qsTr("Media Streams")
            visible: false
            width: Style.scaleX(25)
            height: Style.scaleY(45)
            dataGrid: -1
            dataGridLabel:"mediaStreams"
            extended: false
            Component.onCompleted: {
                parent = fp_panel.headerRow
            }
            onExtendedChanged: refresh()

            delegate:
                StyledButton{
                id:gridBtn
                buttonText: description
                textSize: Style.appFontSize_description
                height: Style.appButtonHeight
                width: parent.width

                onActivated: {
                    var eas = ''
                    var i = 0;
                    for (var prop in floorplan_devices.getSelectedDevices()){
                        if (i > 0)
                            eas += ','
                        eas += prop
                        i++;
                    }
                    if (i > 0) {
                        manager.doMoveMedia(eas,streamID)
                    } else {
                        // stop stream by stream id
                    }
                    activeStreams.refresh()
                }
            }
        }



        SendCommandBox {
            id: sendCommandBox
            width: Style.appButtonWidth
            height: Style.scaleY(65)
            Component.onCompleted: parent=fp_panel.headerRow
        }

        ScrollRow{
            id:row_button_scroll
            visible:false
            enabled:visible
            anchors{
                left:phoneFloorplanLayout.left
                right:phoneFloorplanLayout.right
                top:phoneFloorplanLayout.top
            }
            height: Style.appNavigation_panelHeight

            Row {
                id: rowButtons
                anchors.left:parent.left
                anchors.right: parent.right
                spacing:5
                StyledButton {
                    id: btOff
                    height: parent.height
                    width: Style.appButtonWidth/2
                    buttonText: "Stop"
                    // call stopMedia with selected EA
                    onActivated: {
                        for (var ea in floorplan_devices.getSelectedDevices()) {
                            console.log("manager.stopPlayback with EA = " + ea)
                            manager.stopMediaOtherLocation(ea)
                        }
                        activeStreams.refresh()
                    }
                }
                StyledButton {
                    id: btRemote
                    height: Style.appButtonHeight
                    width: 100
                    buttonText: "Remote"
                    // open remote for this EA
                    onActivated: {
                    }
                }
            }
        }

        Item{
            id:bottomControls
            anchors{
                bottom:parent.bottom
                left:parent.left
                right:parent.right
            }
            height: Style.scaleY(8)

            Rectangle{
                anchors.fill: parent
                color: Style.appcolor_background
            }
            ListView{
                width: parent.width
                height:parent.height
                anchors.bottom:parent.bottom
                orientation: ListView.Horizontal
                model:selections
                delegate: StyledButton{
                    width: Style.appButtonWidth
                    height: parent.height
                    buttonText:name
                    onActivated:floorplan_devices.setDeviceSelection(device)
                }
            }
        }

        ControlPopupContainer{
            id:popupControls
        }


        states: [
            State {
                name: "floorplanView"
                when:selections.count==0
                PropertyChanges {
                    target: floorplanPic
                    visible:true
                    opacity:1
                }
                PropertyChanges{
                    target: sendCommandBox
                    opacity:25
                    enabled:false

                }
            },
            State {
                name: "commandView"
                PropertyChanges {
                    target: floorplanPic
                    opacity:.55
                    visible:true
                }
                PropertyChanges{
                    target: sendCommandBox
                    enabled:true
                    opacity:1
                }
            }
        ]

        transitions: [
            Transition {
                from: "*"
                to: "commandView"
                ScriptAction{
                    script: phoneFloorplanLayout.processSelectedDevices()
                }
            },
            Transition {
                from: "*"
                to: "floorplanView"
                ScriptAction{
                    script:phoneFloorplanLayout.forceActiveFocus()
                }
            }
        ]
    }
}

