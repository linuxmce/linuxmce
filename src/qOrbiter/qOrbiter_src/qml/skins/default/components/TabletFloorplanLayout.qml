import QtQuick 2.2
import "../."
Panel{
    id:fp_panel
    headerTitle:qsTr("Floorplan")

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

        ListModel{
            id:selections

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


        Connections{
            target:floorplan_devices
            onSelectedDevicesChanged:processSelectedDevices()
        }


        ParamManager{
            id:requestParamManager
        }

        FloorPlandisplayTemplate{
            id:floorplanPic
            useList: true
        }

        GenericListModel{
            model: floorplan_pages
            label: qsTr("Floorplans")

            Component.onCompleted: parent=fp_panel.headerRow
            width: Style.scaleX(25)
            height: Style.scaleY(45)
            delegate:Rectangle{
                height: Style.scaleY(9)
                width: Style.scaleX(35)
                color: Style.appcolor_background
                border.color: Style.appcolor_foregroundColor
                StyledText {
                    id: desc
                    text: m_description
                    width: parent.width
                    height: parent.height
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


        SendCommandBox {
            id: sendCommandBox
           Component.onCompleted: parent=fp_panel.headerRow
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
                    opacity:0
                    visible:false
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
                    visible:true
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

