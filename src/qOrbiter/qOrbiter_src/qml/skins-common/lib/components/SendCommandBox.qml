import QtQuick 2.2
import "../components"
import "../../qt5/default/components"
Item{
    id:sendCommandBox
   anchors{
   top:parent.top
   left:parent.left
   right:parent.right
   bottom:bottomControls.top
   }
    focus:true
    clip:true
    property Item trackedDevice
    property int trackedInt:0

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

    onActiveFocusChanged: {
        console.log("Cmd box  focus"+activeFocus)
        if(activeFocus && selections.count !==0){
            trackedInt = selections.get(0).device
            manager.getFloorplanDeviceCommands(trackedInt)
        }
    }

    ListModel{
        id:availbleCommands
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

    Rectangle{
        id:phil
        anchors.fill: parent
        color:"black"
        opacity: .85
    }

    Rectangle{
        id:controlsContainer
        height: parent.height*.75
        width: parent.width*.95
        color: androidSystem.blueStandard
        radius: 5
        border.color: "white"
        anchors.centerIn: parent
        visible:parent.visible
    }

    Text {
        id: cmdsPrmoptText
        text: qsTr("Availible Commands")
        font.pointSize:scaleY(3)
        color: "black"
        font.bold: true
        anchors.top: controlsContainer.top
        anchors.horizontalCenter: controlsContainer.horizontalCenter
        anchors.topMargin: 10
    }
    ListView{
        id:cmdView
        anchors.top: cmdsPrmoptText.bottom
        width: controlsContainer.width -5
        height: controlsContainer.height - cmdsPrmoptText.height - 10
        anchors.horizontalCenter: controlsContainer.horizontalCenter
        model:availbleCommands
        clip:true
        spacing:scaleY(2)
        delegate: Loader {
            sourceComponent: getCmdDelegate(command_number)
            property variant cmdName : command_name
            property variant cmdNumber : command_number
        }
    }

    function containsCmd(cmdNo) {
        for(var i = 0; i < cmdView.model.count; i++) {
            if (cmdView.model.get(i).command_number == cmdNo)
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
            height: style.buttonHeight+6
            width: cmdView.width
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5
            StyledText {
                anchors.left: parent.left
                text: "On/Off"
            }
            Row {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                spacing: 10
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
                    label: "On"
                    onActivated: sendCommand(cmdNumber, []);
                }
                StyledButton {
                    anchors.verticalCenter: parent.verticalCenter
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
            height: style.buttonHeight+6
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
            height: style.buttonHeight+6
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
            height: style.buttonHeight+6
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
                    height: style.buttonHeight+6
                    width: cmdView.width
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
                                                              "value":""
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

                    Text{
                        id:lbl
                        text:cmdName
                        font.pointSize: 24
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


                            height: state==="selected" !==0 ? 60 : 0
                            width: parent .height !==0 ? 120 : 0
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
                                    onValChanged: setParam(String(val))
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

                        Text{
                            text: "Send"
                            color:"white"
                            font.pointSize: 18
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
