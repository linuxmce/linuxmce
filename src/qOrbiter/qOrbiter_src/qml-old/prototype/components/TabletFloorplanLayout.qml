import QtQuick 1.0
import "../../../../skins-common/lib/components"
Item{
    id:phoneFloorplanLayout
    height: manager.appHeight
    width:manager.appWidth
    focus:true
    property bool rotated:manager.isProfile
    property string imageDir:""
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
    }

    ListView{
        width: parent.width
        height: scaleY(10)
        anchors.bottom: parent.bottom
        orientation: ListView.Horizontal
        model:selections
        delegate: StyledText{
            width: 150
            height: 50
            text:name
            color:"white"
            MouseArea{
                anchors.fill: parent
                onReleased:floorplan_devices.setDeviceSelection(device)
            }
        }
    }

    SendCommandBox {
        id: sendCommandBox
    }

    HomeButton{ x: 5; y: 5; width: 75; height: 75; smooth: true}

    states: [
        State {
            name: "floorplanView"
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
                opacity:0
                visible:false
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
                script: sendCommandBox.forceActiveFocus()
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
