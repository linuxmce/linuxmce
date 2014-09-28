import QtQuick 1.1

ListView
{
    property int floorplanType:0
    id:floorplanDevices
    width: scaleX(100)
    height: scaleY(100)
    model:floorplan_devices
    anchors.top: parent.top
    anchors.left: parent.left
    visible:parent.state ==="list"
    clip:true
    focus:true

    orientation: ListView.Vertical
    spacing:scaleY(1)
    delegate:Item{
        id:fp_device_rect
        height: scaleY(26)
        width: parent.width
        property bool activated:floorplanDevices.currentIndex === index
        Component.onCompleted:  manager.getFloorplanDeviceCommands(deviceno);
        Keys.onPressed: {
            if(event.key===Qt.Key_Left){
                fp_submodel.decrementCurrentIndex()
            } else if(event.key===Qt.Key_Right){
                console.log(fp_submodel.currentIndex)
                if(fp_submodel.currentIndex !== fp_submodel.count-1)
                    fp_submodel.incrementCurrentIndex()
                else
                    chkBox.forceActiveFocus()
            }
            else if (event.key === Qt.Key_Space){
                requestParamManager.getParams(commandlist["commands"][fp_submodel.currentIndex].command_number, deviceno)
                parameters.forceActiveFocus()
            }
        }

        Rectangle{
            id:phil
            anchors.fill: parent
            color:mickey.pressed ? "orange" : floorplanDevices.currentIndex === index ? "darkorange" : "Black"
            opacity: .75

        }
        MouseArea{
            id:mickey
            anchors.fill: parent
            onClicked: {manager.getFloorplanDeviceCommands(deviceno); floorplanDevices.currentIndex = index; chkBox.trip()}
        }
        Column{
            width: parent.width
            height: parent.height

            StyledText {
                id: fpDevice_name
                text: name
                fontSize: fp_device_rect.activated ? scaleY(5) : scaleY(8)
                color:"white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ListView{
                id:fp_submodel
                width: parent.width
                height: scaleY(8)
                visible:fp_device_rect.activated
                model:commandlist["commands"]
                orientation:ListView.Horizontal
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:scaleX(2)
                delegate:StyledButton{
                    buttonText: commandlist["commands"][index].command_name
                    hitArea.onReleased: {fp_submodel.currentIndex = index;
                        console.log(JSON.stringify(commandlist["commands"][index]));
                        requestParamManager.getParams(commandlist["commands"][index].command_number, deviceno)
                    }

                }
            }


            ListView{
                id:parameters
                height: scaleY(12)
                width: scaleX(75)
                model: paramlist.length===0 ? undefined :paramlist
                spacing:scaleX(8)
                visible: fp_device_rect.activated
                anchors.horizontalCenter: parent.horizontalCenter
                orientation: ListView.Horizontal
                clip:true
                delegate:
                    Item{
                    height: parameters.height
                    width: plabel.paintedWidth
                    property string value:stringbox.text
                    onActiveFocusChanged: {
                        if(activeFocus)
                            stringbox.forceActiveFocus()
                    }

                    Column{
                        height:parent.height
                        width: scaleX(5)
                        Item{
                            id:entryItem
                            height: parent.height*.55
                            width: parent.height
                            Rectangle{
                                anchors.fill: parent
                                color:"white"
                                radius:5
                            }

                            TextInput{
                                id:stringbox
                                width: parent.width
                                height: parent.height
                                font.pixelSize: scaleY(4)
                                fillColor: "white"
                                text:parameters.model[index].value
                                focus:true
                                Keys.onPressed:
                                    if(event.key=== Qt.Key_Enter || event.key === Qt.Key_Return ){
                                        var b= paramlist
                                        b[index].value=stringbox.text
                                        floorplan_devices.setDeviceParams(b, deviceno)
                                        stringbox.text = b[index].value
                                        console.log("updated")
                                    }
                                    else{
                                        console.log(event.key)
                                    }
                            }
                        }

                        StyledText{
                            id:plabel
                            text: parameters.model[index].CommandDescription
                            fontSize: scaleY(4)
                            color:"white"
                            font.bold: true
                        }
                    }
                }
            }

            StyledButton{
                id:sendCmdBtn
                height:scaleY(6)
                width: scaleX(6)
                visible:floorplanDevices.currentIndex===index
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                buttonText: qsTr("Send")
                textSize: scaleY(8)
                hitArea.onReleased: send()
                function send(){
                    console.log(JSON.stringify(floorplan_devices.selectedDevices))
                    var t = paramlist
                    var commandObj = {};
                    commandObj.command =commandlist["commands"][fp_submodel.currentIndex].command_number
                    var tParams = Array()
                    for (var pp in t){
                        var tpObj = {}
                        tpObj.paramno=t[pp].CommandParameter
                        tpObj.val=t[pp].value
                        tParams.push(tpObj)
                    }
                    commandObj.params = tParams;
                    commandObj.count= t.length
                    commandObj.to = floorplan_devices.selectedDevices
                    manager.sendDceMessage(commandObj)
                }
            }

            Connections{
                target: parameters
                // onCountChanged:console.log("Param Model==>"+JSON.stringify(parameters.model))
            }
        }

        Rectangle{
            id:chkBox
            color: "red"
            height: scaleY(8)
            width: scaleY(8)
            radius: 2
            border.color: activeFocus ? "white" : "orange"
            anchors.right: parent.right
            anchors.top: parent.top

            function trip(){
                floorplan_devices.setDeviceSelection(deviceno)
            }

            Rectangle{
                id:select_indicator
                height: selected ? parent.height  : 0
                width: selected ? parent.width : 0
                color:"green"
                anchors.centerIn: parent
            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    chkBox.trip()
                }
            }
        }

    }
}



