import QtQuick 2.2
import "../components"
import "../."
GenericListModel{
    id:sendCommandBox
    extended:false
    height: Style.scaleY(75)
    width: Style.scaleX(25)
    focus:true
    clip:true
    model:availbleCommands
    label:qsTr("Availible Commands")
    property Item trackedDevice
    property int trackedInt:0
    delegate: StyledButton{
        width: parent.width
        height: Style.listViewItemHeight
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

    Component {
        id: onOffDelegate

        Rectangle {
            id:cmdEntry
            height: Style.appButtonHeight
            width: parent.width
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5
            StyledText {
                anchors.left: parent.left
                text: "On/Off"
                fontSize: Style.appFontSize_description
            }
            Row {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                spacing: 10
                StyledButton {
                    height: Style.appButtonHeight
                    width: Style.appButtonWidth /2
                    textSize: Style.appFontSize_description
                    anchors.verticalCenter: parent.verticalCenter
                    label:qsTr( "On")
                    onActivated: sendCommand(cmdNumber, []);
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    height: Style.appButtonHeight
                    fontSize: Style.appFontSize_description
                    width: Style.appButtonWidth /2
                    label: "Off"
                    onActivated: sendCommand("193", []);
                }
            }

        }
    }

    Component {
        id: setFanDelegate

        Rectangle {
            id:cmdEntry
            height: Style.appButtonHeight
            width: cmdView.width
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5
            function sendFanCmd(param){
                sendCommand(cmdNumber, [{'paramno': 5, 'val': param }]);
            }
            StyledText {
                anchors.left: parent.left
                text: cmdName
            }
            Row {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                spacing: 10
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "Auto"
                    onActivated: sendTempCmd("0");
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "On"
                    onActivated: sendTempCmd("1");
                }
            }

        }
    }

    Component {
        id: setTemperatureDelegate

        Rectangle {
            id:cmdEntry
            height: Style.appButtonHeight
            width: cmdView.width
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5
            function sendTempCmd(param){
                sendCommand(cmdNumber, [{'paramno': 5, 'val': param }]);
            }
            StyledText {
                anchors.left: parent.left
                text: cmdName
            }
            Row {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                spacing: 10
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "-2"
                    onActivated: sendTempCmd("-2");
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "-1"
                    onActivated: sendTempCmd("-1");
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "+1"
                    onActivated: sendTempCmd("+1");
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "+2"
                    onActivated: sendTempCmd("+2");
                }
            }

        }
    }

    Component {
        id: setHeatCoolDelegate
        Rectangle {
            id:cmdEntry
            height: Style.appButtonHeight
            width: cmdView.width
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5
            function sendCmdHeatCool(param){
                sendCommand(cmdNumber, [{'paramno': 8, 'val': param }]);
            }
            StyledText {
                anchors.left: parent.left
                text: cmdName
            }
            Row {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                spacing: 10
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "Heat"
                    onActivated: sendCmdHeatCool("H");
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "Cool"
                    onActivated: sendCmdHeatCool("C");
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "Auto"
                    onActivated: sendCmdHeatCool("A");
                }
            }
        }
    }

    Component {
        id: defaultCmdDelegate
        Rectangle{
            id:cmdEntry
            height: Style.appButtonHeight
            width: parent.width
            state:"preselect"
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5

            property bool isActive:false


            function sendCommand(){
                var commandObj = {};

                commandObj.command = cmdNumber

                var tParams = new Array
                for (var i = 0; i < paramCache.count; i++){
                    var t = paramCache.get(i)

                    var tpObj = {}
                    tpObj.paramno=t.CommandParameter
                    tpObj.val=t.value
                    tParams.push(tpObj)
                }
                commandObj.params = tParams;
                commandObj.count= paramCache.count
                commandObj.to = floorplan_devices.selectedDevices
                manager.sendDceMessage(commandObj)
            }

            Connections{
                target:floorplan_devices

                onDeviceParamsChanged:{
                    if(cmdEntry.state==="selected"){
                        paramCache.clear()
                        console.log("Command Params updated")
                        var cl = floorplan_devices.getCommandParams(trackedInt)
                        for(var c in cl){
                            console.log(cl[c].Command+"::"+cmdNumber)
                            if(cl[c].Command==cmdNumber){
                                console.log(JSON.stringify(cl[c]))
                                paramCache.append({
                                                      "Command":cl[c].Command,
                                                      "CommandDescription":cl[c].CommandDescription,
                                                      "CommandHint":cl[c].CommandHint,
                                                      "CommandParameter":cl[c].CommandParameter,
                                                      "type":cl[c].type,
                                                      "value":undefined
                                                  } )
                            }
                        }
                    }
                }
            }
            ListModel{
                id:paramCache
            }

            Timer{
                id:entry_timeout
                running:false
                repeat: true
                interval:5000
                onTriggered:{
                    if(!cmdEntry.isActive)
                    {parent.state="preselect"}

                }
            }

            StyledText{
                id:lbl
                text:cmdName
                fontSize: Style.appFontSize_description
                color:"white"

            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    requestParamManager.getParams(cmdNumber, trackedInt);
                    cmdEntry.state="selected";entry_timeout.start()
                }
            }

            ListView{
                id:params
                model:paramCache
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                clip:true
                delegate: Item{
                    id:cmdDelegate
                    clip:false
                    property string sourceString:controls_loader.source
                    Component.onCompleted: {
                        if(Command==="192"){
                            if(CommandParameter!=98){
                                setParam(0)
                                controls_loader.source = "OnButton.qml"
                            }
                        }
                        else if(Command==="193"){
                            if(CommandParameter!=98){
                                setParam(0)
                                controls_loader.source = "OffButton.qml"
                            }

                        }
                        else if(Command==="184"){
                            if(CommandParameter==="76"){
                                controls_loader.source = "Slider.qml"
                            }
                        } else if(Command==="279") { // Set Fan
                            // controls_loader.source = "SlideOnOff?"
                        }
                        else if(Command==="641"){
                            controls_loader.source = "OnOffSwitch.qml"
                        }
                        else if(Command==="980"){

                            controls_loader.source="ColorSlider.qml"

                        }
                    }

                    function setParam(val){
                        paramCache.set(model.index, {"value":val})
                    }

                    function setActive(b){
                        cmdEntry.isActive = b
                        entry_timeout.restart()
                    }

                    height:Style.appButtonHeight
                    width: parent.width
                    Column{
                        spacing:5
                        width: parent.width
                        height: parent.height
                        Text{
                            text:CommandDescription
                            color:"white"
                        }
                        Loader{
                            id:controls_loader
                            property int val
                            onValChanged: cmdDelegate.setParam(String(val))
                        }
                    }
                }
            }

            Rectangle{
                id:sndButton
                height:60
                width: 120
                radius: 5
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                color: ms.pressed ? "green": "red"

                StyledText{
                    text: "Send"
                    color:"white"
                    fontSize: Style.appFontSize_description
                }

                MouseArea{
                    id:ms
                    anchors.fill: parent
                    onClicked: cmdEntry.sendCommand()
                }
            }

            states: [
                State {
                    name: "preselect"

                    PropertyChanges {
                        target: cmdEntry
                        height:75
                    }
                    PropertyChanges {
                        target: sndButton
                        visible:false
                    }
                    AnchorChanges {
                        target: lbl
                        anchors.left: undefined
                        anchors.verticalCenter:cmdEntry.verticalCenter
                        anchors.horizontalCenter: cmdEntry.horizontalCenter
                    }
                    PropertyChanges {
                        target: params
                        height:0
                        width:0
                    }
                    PropertyChanges {
                        target: entry_timeout
                        running:false
                    }
                },
                State{
                    name:"selected"
                    PropertyChanges {
                        target: cmdEntry
                        height:params.height
                    }
                    PropertyChanges {
                        target: sndButton
                        visible:false
                    }
                    AnchorChanges {
                        target: lbl
                        anchors.verticalCenter:undefined
                        anchors.horizontalCenter: undefined
                        anchors.left: cmdEntry.left

                    }
                    PropertyChanges {
                        target: params
                        height:(params.count+1)*60
                        width:cmdEntry.width*.65
                    }
                    PropertyChanges {
                        target: entry_timeout
                        running:true
                    }
                }

            ]
        }

    }
}
