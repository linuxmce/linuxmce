import QtQuick 1.0
import "../components"
Item{
    id:sendCommandBox
    height: manager.appHeight
    width: manager.appWidth
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
        if(activeFocus){
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
        color: androidSystem.greenStandard
        radius: 5
        border.color: "white"
        anchors.centerIn: parent
        visible:parent.visible
    }

    Text {
        id: cmdsPrmoptText
        text: qsTr("Availible Commands")
        font.pointSize: scaleY(3)
        color: "Black"
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
        delegate: Rectangle{
            id:cmdEntry
            height: 75
            width: cmdView.width
            state:"preselect"
            color: "black"
            border.color: "white"
            border.width: 1
            radius:5

            function sendCommand(){
                var commandObj = {};

                commandObj.command = command_number

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
                            console.log(cl[c].Command+"::"+command_number)
                            if(cl[c].Command==command_number){
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
                repeat: false
                interval:5000
                onTriggered: parent.state="preselect"
            }

            Text{
                id:lbl
                text:command_name
                font.pointSize:24
                color:"white"

            }

            MouseArea{
                anchors.fill: parent
                onClicked: {requestParamManager.getParams(command_number, trackedInt); cmdEntry.state="selected";entry_timeout.start() }
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
                            setParam(0)
                        }
                        else if(Command==="193"){
                            setParam(0)
                        }
                        else if(Command==="184"){
                            controls_loader.source = "Slider.qml"
                            setParam(0)
                        }
                    }

                    function setParam(val){
                        paramCache.set(model.index, {"value":val})
                    }


                    height: parent.height !==0 ? 60 : 0
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

            ButtonSq{
                id:sndButton
                buttontext: "Send"
                height:60
                width: 120
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                onActivated: cmdEntry.sendCommand()
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
                        height:childrenRect.height
                    }
                    PropertyChanges {
                        target: sndButton
                        visible:true
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
