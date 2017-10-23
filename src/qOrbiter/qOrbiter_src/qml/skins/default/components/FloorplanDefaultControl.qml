import QtQuick 2.2
import "../components"
import "../."
Item{
    id:cmdEntry
    anchors.centerIn: parent

    Component.onCompleted: {
        requestParamManager.getParams(cmdNumber, trackedInt);
    }

    Rectangle{
        anchors.fill: parent
        color: "black"
        opacity:appStyle.appHeader_opacity
    }

    state:"selected"

    
    property bool isActive:state==="selected"
    
    
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
    MouseArea{
        anchors.fill: parent
        onClicked: {
            controls_loader.source=undefined
            requestParamManager.getParams(cmdNumber, trackedInt);

        }
    }
    
    ListView{
        id:params
        model:paramCache

      anchors{
       top:parent.top
       left:parent.left
       right:parent.right
       bottom:parent.bottom
       margins: 5
      }
        clip:true
        delegate: Item{
            id:cmdDelegate
            clip:false
            height:appStyle.appButtonHeight
            width: parent.width

            StyledText{
                id:desc
                text:CommandHint
                width: parent.width/2
                color:"white"
                fontSize: appStyle.appFontSize_description
                anchors{
                    top:parent.top
                    horizontalCenter: parent.horizontalCenter
                }
            }
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
                
            }

            Loader{

                width:controls_loader.item.width
                height: appStyle.listViewItemHeight
                anchors{
                    top:desc.bottom
                    horizontalCenter: parent.horizontalCenter
                }

                id:controls_loader
                property int val
                onValChanged: cmdDelegate.setParam(String(val))

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
            fontSize: appStyle.appFontSize_description
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
        }
        
    ]
}
