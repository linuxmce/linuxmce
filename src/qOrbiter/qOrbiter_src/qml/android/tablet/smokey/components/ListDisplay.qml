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
    clip:true
    focus:true

    orientation: ListView.Vertical
    spacing:scaleY(1)
    delegate:Item{
        id:fp_device_rect
        height: scaleY(26)
        width: parent.width
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
            onClicked: {manager.getFloorplanDeviceCommands(deviceno); floorplanDevices.currentIndex = index}
        }

        Column{
            width: parent.width
            height: parent.height

            StyledText {
                id: fpDevice_name
                text: name
                fontSize: headerText
                color:"white"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ListView{
                id:fp_submodel
                width: parent.width
                height: scaleY(8)
                model:commandlist["commands"]
                orientation:ListView.Horizontal
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:scaleX(2)
                delegate:StyledButton{
                    buttonText.text: commandlist["commands"][index].command_name
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

                                Keys.onTabPressed: {
                                    var b= paramlist
                                    b[index].value=stringbox.text
                                    floorplan_devices.setDeviceParams(b, deviceno)
                                    stringbox.text = b[index].value
                                    console.log("updated")

                                    if(activeFocus && parameters.currentIndex !== parameters.count-1){
                                        parameters.incrementCurrentIndex()
                                    }
                                    else{
                                        sendCmdBtn.send()
                                    }

                                }


                                Keys.onPressed:
                                    if(event.key=== Qt.Key_Enter || event.key === Qt.Key_Return){
                                        var b= paramlist
                                        b[index].value=stringbox.text
                                        floorplan_devices.setDeviceParams(b, deviceno)
                                        stringbox.text = b[index].value
                                        console.log("updated")
                                    }

                                //                                onActiveFocusChanged: {
                                //                                    if(text.length!==0){
                                //                                        var b= paramlist
                                //                                        b[index].value=stringbox.text
                                //                                        floorplan_devices.setDeviceParams(b, deviceno)
                                //                                        stringbox.text = b[index].value
                                //                                        console.log("updated")
                                //                                    }
                                //                            }


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

            Item{
                id:sendCmdBtn
                height:scaleY(6)
                width: scaleX(6)
                visible:floorplanDevices.currentIndex===index
                anchors.right: parent.right
                anchors.bottom: parent.bottom

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

                Rectangle{
                    anchors.fill: parent
                    color: "black"
                    radius: 5
                }
                StyledText{
                    text: "Send"
                    fontSize: scaleY(4)
                    color:"white"
                    font.bold: true
                }

                MouseArea{
                    anchors.fill: parent
                    onClicked: {

                        sendCmdBtn.send()
                    }
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
            height: scaleY(5)
            width: scaleY(5)
            radius: 2
            border.color: activeFocus ? "white" : "red"
            anchors.right: parent.right
            anchors.top: parent.top
            Rectangle{
                id:select_indicator
                height: selected ? parent.height  : 0
                width: selected ? parent.width : 0
                color:"green"
                anchors.centerIn: parent
            }
            Keys.onPressed: {
                if(event.key===Qt.Key_Enter || event.key=== Qt.Key_Return){
                    floorplan_devices.setDeviceSelection(deviceno)
                    return;
                } else if(event.key ===Qt.Key_Left){
                    fp_device_rect.forceActiveFocus()
                    return;
                }else if(event.key === Qt.Key_Up || event.key=== Qt.Key_Down ){
                    if(event.key===Qt.Key_Up){
                        floorplanDevices.decrementCurrentIndex()
                    }
                    else if (event.key === Qt.Key_Down){
                        floorplanDevices.incrementCurrentIndex()
                    }
                    return
                }

            }

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    parent.forceActiveFocus()
                    floorplan_devices.setDeviceSelection(deviceno)
                }
            }

        }
    }
}



