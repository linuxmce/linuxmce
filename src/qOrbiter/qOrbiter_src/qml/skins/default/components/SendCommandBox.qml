import QtQuick 2.2
import "../components"
import "../."
GenericListModel{
    id:sendCommandBox
    extended:false
    height: appStyle.scaleY(75)
    width: appStyle.scaleX(25)
    focus:true
    clip:true
    model:availbleCommands
    label:qsTr("Commands")
    property Item trackedDevice
    property int trackedInt:0
    delegate: StyledButton{
        width: parent.width
        height: appStyle.listViewItemHeight
        buttonText: command_name
        //        Loader {
        //            id:control_comp
        //            anchors.fill: parent
        //            sourceComponent: getCmdDelegate(command_number)
        //            property variant cmdName : command_name
        //            property variant cmdNumber : command_number
        //        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                var comp= getCmdDelegate(command_number);
                popupControls.setCommandDelegate(comp, index )
                sendCommandBox.extended=false
            }
        }
    }

    ListModel{
        id:availbleCommands
    }
    Connections{
        target:floorplan_devices
        onDeviceCommandsChanged:{
            console.log(JSON.stringify(floorplan_devices.getDeviceCommands(trackedInt)))
            availbleCommands.clear()
            var cl = floorplan_devices.getDeviceCommands(trackedInt).commands
            for(var c in cl){
                availbleCommands.append(cl[c])
            }
        }
    }

    function refreshCommands(){
        if(selections.count==0)
            return;

        trackedInt = selections.get(0).device
        manager.getFloorplanDeviceCommands(trackedInt)
    }

    Keys.onReleased: {
        event.accepted = true
        switch(event.key){

        case Qt.Key_Menu:
            phoneFloorplanLayout.state="floorplanView"
            console.log("hiding command view")
            break;
        default:
            console.log("Cmd box Recieved key ==>"+event.key+" but not accepting")
            break;
        }
    }


    function containsCmd(cmdNo) {
        for(var i = 0; i < sendCommandBox.count; i++) {
            if (sendCommandBox.model.get(i).command_number == cmdNo)
                return true
        }
        return false;
    }

    function getCmdDelegate(cmdNo) {
        if (cmdNo == "280")
            return setHeatCoolDelegate;
        else if (cmdNo == "278")
            return setTemperatureDelegate;
        else if (cmdNo == "279")
            return setFanDelegate;
        else if (cmdNo == "192" && containsCmd("193"))
            return onOffDelegate;
        else if (cmdNo == "193" && containsCmd("192"))
            return null;
        else
            return defaultCmdDelegate;
    }

    function sendCommand(cmdNo, params) {
        var commandObj = {};
        commandObj.command = cmdNo
        commandObj.params = params;
        commandObj.count= params.length;
        commandObj.to = floorplan_devices.selectedDevices
        manager.sendDceMessage(commandObj)
    }

    Component { id: onOffDelegate; FloorplanOnOff { id:on_off } }
    Component { id: setFanDelegate; FloorplanFanSwitch { id: fan_sw } }
    Component { id: setTemperatureDelegate; FloorplanSetTemp { id: set_temp } }
    Component { id: setHeatCoolDelegate;  FloorplanHeatCool { id: heat_cool } }
    Component { id: defaultCmdDelegate ; FloorplanDefaultControl { id: default_control } }
}
