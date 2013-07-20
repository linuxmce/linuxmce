import QtQuick 1.1

ListView
{
    property int floorplanType:0
    id:floorplanDevices
    width: scaleX(85)
    height: scaleY(85)
    model:floorplan_devices
    anchors.top: parent.top
    anchors.left: parent.left
    clip:true
    focus:true
    orientation: ListView.Vertical
    spacing:scaleY(1)
    delegate:Item{
        id:fp_device_rect
        height: floorplanDevices.currentIndex === index ? scaleY(26) : fpDevice_name.paintedHeight
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
        }

        Rectangle{
            id:phil
            anchors.fill: parent
            color:mickey.pressed ? skinStyle.darkHighlightColor :skinStyle.mainColor
            opacity: .75

        }



        //            Column{
        //                id:infocol
        //                height: parent.height
        //                width: parent.width*.25
        //                anchors.right: parent.right
        //                anchors.top: parent.top

        //                Text {
        //                    id: fpDevice_type
        //                    text: "I am type" + type
        //                    color:"white"
        //                }
        //                Text {
        //                    id: fp_type
        //                    text: "Floorplan type" + floorplantype
        //                    color:"white"

        //                }
        //                Text {
        //                    id: fpDevice_no
        //                    text: "I am Dev#" + deviceno
        //                    color:"white"
        //                }
        //                Text {
        //                    id: fpDevice_pos
        //                    text: "Position" + floorplan_devices.getDeviceX(deviceno) + "," + floorplan_devices.getDeviceY(deviceno)
        //                    color:"white"
        //                    // onTextChanged: placeSprites(floorplan_devices.getDeviceX(deviceno),floorplan_devices.getDeviceY(deviceno),deviceno)
        //                }
        //            }
        MouseArea{
            id:mickey
            anchors.fill: parent
            onClicked: manager.getFloorplanDeviceCommands(deviceno)
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
                opacity: floorplanDevices.currentIndex === index ? 1 : 0
                width: parent.width
                height: parent.height/2
                model:commandlist["commands"]
                orientation:ListView.Horizontal
                anchors.horizontalCenter: parent.horizontalCenter
                spacing:scaleX(2)
                delegate: Rectangle{
                    id:thisone
                    height:50
                    width: lbl.width
                    radius:5
                    color: fp_submodel.currentIndex === index ? skinStyle.mainColor : sublist_hit.containsMouse ? "lightblue": sublist_hit.pressed ? "green": "darkblue"
                    StyledText{
                        id:lbl
                        text: commandlist["commands"][index].command_name
                        anchors.centerIn: parent
                        fontSize: mediumText
                        font.bold: true
                        color:"white"
                    }
                    MouseArea{
                        id:sublist_hit
                        anchors.fill: thisone
                        onClicked: {console.log("Grabbed"); console.log(JSON.stringify(commandlist["commands"][index])); requestParamManager.getParams(commandlist["commands"][index].command_number, deviceno) }

                    }
                }
            }
            ListView{
                id:parameters
                height: floorplanDevices.currentIndex === index ? scaleY(8) : 0
                width: parent.width
                model:paramlist
                spacing:scaleX(2)
                orientation: ListView.Horizontal

                clip:true
                delegate:
                    Item{
                    height: parameters.height
                    width: childrenRect.width

                    StyledText{
                        text: parameters.model[index].CommandDescription
                        fontSize: mediumText
                        color:"white"
                        font.bold: true
                    }
                }


            }
            Connections{
                target: parameters
                onCountChanged:console.log("Param Model==>"+JSON.stringify(parameters.model))
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
                height: selected ? parent.height -1 : 0
                width: selected ? parent.width -1 : 0
                color:"green"
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


