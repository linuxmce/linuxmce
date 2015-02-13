import QtQuick 2.2
import "../../../../skins-common/lib/components"
Item{
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

    function toggleCommands() {
        state= (state == "commandView" ? "floorplanView" : "commandView");
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
        useList: false
    }

    ListView{
        width: parent.width
        height: scaleY(10)
        anchors.bottom:bottomControls.top
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

    Item{
        id:bottomControls
        anchors{
            bottom:parent.bottom
            left:parent.left
            right:parent.right
        }
        height: scaleY(8)

        Rectangle{
            anchors.fill: parent
            color: "green"
        }

        Image {
            id: zoomOut
            source: "../images/zoom_out.png"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: -40
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    floorplanPic.setScaleFactor(-15)
                }
            }
        }
        Image {
            id: zoomIn
            source: "../images/zoom_in.png"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: -40
            MouseArea{
                anchors.fill: parent
                onClicked: {
                   floorplanPic.setScaleFactor(15)
                }
            }
        }
    }



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
